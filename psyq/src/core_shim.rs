//! RESTORE: PLATFORM. The one seam between the layer and the SDK's resident scene core.
//!
//! Every place the games touch the core goes through here: the drive (every sector the layer reads),
//! loading a scene image into its fixed range, the scene the handoff block reports, and a stream
//! client taking the drive (the film). The names and meanings follow the core's ABI (scene-system
//! CORE-ABI.md section 3: `psx_core_read`, `psx_core_pause`, `psx_core_client_attach` and
//! `_detach`, `psx_core_load_image`). Declared for C in `psyq/include/psx_core_shim.h`.
//!
//! The scene images load through the SDK's core (`psx_core::link::load_image`); the stream client
//! (the film) attaches through it. The game's own files are still read with the SDK's polled sector
//! reader, as the layer always has: the core's read has no read-ahead sector, which answers 65 of
//! 360X's Mission 1's 135 file starts with no drive start (asked of the SDK). The core proves the
//! drive stopped before each of its own reads, whatever the reader left.
//!
//! What stays the layer's, above this seam: the libcd API and the ISO walk (`cd.rs`), the LOAD FAILED
//! screen (`loadfail.rs`), and which image a wrapped entry needs (`scenes.rs`).

use psx_io::cd::reader::{SectorReader, SECTOR_WORDS};
use psx_io::cd::wait::{WaitTime, FIRST_SECTOR_WAIT};
use psx_io::periph::Cd;

/// Bound on the SeekL wait (a full-stroke seek at double speed fits): the SDK's own bound for a seek or a
/// read's first sector, in time (`psx_io::cd::wait`).
const SEEK_POLL: WaitTime = FIRST_SECTOR_WAIT;

/// Tries a read gets: the first and four more, each after a full re-sync of the drive.
///
/// RESTORE: PLATFORM. On a console a read can fail where the emulator never fails one: after the
/// drive has idled (the console measured 4.4 s to the first sector after a Pause and 15 s idle,
/// silicon v2.7) a seek or the first sector can take longer than the SDK reader's spin-count
/// bounds. Now it is tried again (`resync`), and only after the last try is it the caller's
/// failure; the games then show which file, step and error (`loadfail.rs`).
const READ_TRIES: u32 = 5;

/// Stage codes, the core's (CORE-ABI.md 3.2): 0 is success.
pub const STAGE_READ_FAILED: u32 = 0x13;
pub const STAGE_CHECKSUM: u32 = 0x14;
pub const STAGE_NO_ROW: u32 = 0x22;
pub const STAGE_BOUNDS: u32 = 0x24;
pub const STAGE_CLIENT: u32 = 0x31;

/// Where a read stopped (`Failure::step`).
pub(crate) const STEP_PREPARE: u8 = 1; // the SetMode handshake (`SectorReader::prepare`)
pub(crate) const STEP_SEEK: u8 = 2; // SetLoc, SeekL waited to completion, SetMode, ReadN
pub(crate) const STEP_SECTOR: u8 = 3; // a sector of the running stream

/// The last read that failed for good: where it stopped, the reader's diagnostic snapshot
/// (`SectorReader::diagnostics`: cause, status, command, code), the sector and the tries made.
#[derive(Clone, Copy)]
pub(crate) struct Failure {
    pub step: u8,
    pub tries: u8,
    pub diag: u32,
    pub lba: u32,
}

// SAFETY (all statics below): one thread of control, and none of them is touched from the VBlank
// handler.
static mut READER: SectorReader = SectorReader::with_cd(unsafe { Cd::steal() });
static mut LAST_FAILURE: Option<Failure> = None;
/// One sector: directory reads (`sector`) and the sector after a read (`core_read`).
static mut SCRATCH: [u32; SECTOR_WORDS] = [0; SECTOR_WORDS];
/// The sector `SCRATCH` holds (`u32::MAX`: none).
static mut SCRATCH_LBA: u32 = u32::MAX;
/// The disc's size in sectors, from the volume descriptor (0 until it has been read): the sector
/// after a read is only read when it is on the disc.
static mut VOLUME_END: u32 = 0;
/// A stream client (the film) holds the drive.
static mut CLIENT: bool = false;

fn reader() -> &'static mut SectorReader {
    // SAFETY: see the statics.
    unsafe { &mut *(&raw mut READER) }
}

/// The last read that failed after every try, if the last read did.
pub(crate) fn last_failure() -> Option<Failure> {
    // SAFETY: see the statics.
    unsafe { LAST_FAILURE }
}

/// A full re-sync between tries: a fresh reader, so the next `prepare` settles the drive again
/// (asks what it is doing and stops it, proved) instead of trusting what the failed read left.
fn resync() {
    reader().stop();
    // SAFETY: the one reader is replaced; nothing borrows it across this call.
    unsafe {
        *(&raw mut READER) = SectorReader::with_cd(Cd::steal());
        SCRATCH_LBA = u32::MAX;
    }
}

/// One try of `read`: the step and sector it stopped at when it fails.
fn read_once(lba: u32, count: u32, dst: *mut u32, ahead: bool, hook: Option<extern "C" fn(u32)>) -> Result<(), (u8, u32)> {
    let rd = reader();
    if !rd.prepare() {
        return Err((STEP_PREPARE, lba));
    }
    if !rd.start_read_seek_first(lba, fault::seek_poll()) {
        return Err((STEP_SEEK, lba));
    }
    for i in 0..count as usize {
        // SAFETY: the caller's buffer holds `count` whole sectors and is word aligned.
        let sector = unsafe { &mut *(dst.add(i * SECTOR_WORDS) as *mut [u32; SECTOR_WORDS]) };
        if !rd.read_sector(sector) {
            return Err((STEP_SECTOR, lba + i as u32));
        }
        if let Some(hook) = hook {
            hook(i as u32);
        }
    }
    // SAFETY: see the statics.
    if ahead && lba + count < unsafe { VOLUME_END } {
        // RESTORE: PLATFORM. A game that loads its files one after another asks for the next
        // one at the sector this read ends at, and starting a read costs 40 ms to a
        // sector's 6.7 ms: the stream runs on for that one sector, if the disc has it (a read
        // past the last sector of the image answers with an error or nothing, and the layer
        // would wait for it). The core's read has no such option yet: asked for (CORE-ABI.md 3).
        // SAFETY: see the statics.
        unsafe {
            SCRATCH_LBA = u32::MAX;
            if rd.read_sector(&mut *(&raw mut SCRATCH)) {
                SCRATCH_LBA = lba + count;
            }
        }
        // a failed extra sector is not the caller's failure: the stream is stopped below
    }
    rd.stop();
    Ok(())
}

/// Read `count` sectors from `lba` into `dst` (whole sectors, word aligned), trying up to
/// `READ_TRIES` times with a full re-sync between tries. With `ahead`, the stream runs on for the
/// one sector after them, which `core_read` answers a one-sector request for with no drive start.
fn read(lba: u32, count: u32, dst: *mut u32, ahead: bool, hook: Option<extern "C" fn(u32)>) -> bool {
    for tries in 1..=READ_TRIES {
        match read_once(lba, count, dst, ahead, hook) {
            Ok(()) => {
                // SAFETY: see the statics.
                unsafe { LAST_FAILURE = None };
                return true;
            }
            Err((step, at)) => {
                let rd = reader();
                let mut diag = rd.diagnostics();
                if diag == 0 {
                    // The wait for the end of the seek spun out: the reader keeps no snapshot of that
                    // one, so it is made here the way it makes its own (timeout, status, SeekL, flag).
                    let cd = rd.cd_mut();
                    diag = u32::from_be_bytes([0xff, cd.status_register(), 0x15, cd.irq_flag_value()]);
                }
                // SAFETY: see the statics.
                unsafe { LAST_FAILURE = Some(Failure { step, tries: tries as u8, diag, lba: at }) };
                resync();
            }
        }
    }
    false
}

/// Test builds only (feature `test-cd-fault`): a read failure and a long read delay put in at a
/// load the game chooses (`CdTestFault`), through the real failure path.
#[cfg(feature = "test-cd-fault")]
mod fault {
    // SAFETY (both): foreground-only statics.
    static mut FAILS: u32 = 0;
    static mut ARMED: bool = false;

    /// Arm the next `CdRead`: it waits `delay` vblanks first, then its first `fails` tries have
    /// their seek wait cut to one spin, so each fails as a seek that spun out does.
    #[no_mangle]
    pub extern "C" fn CdTestFault(fails: i32, delay: i32) {
        // SAFETY: see the statics.
        unsafe {
            FAILS = fails.max(0) as u32;
            ARMED = true;
        }
        let end = crate::kernel::VSync(-1).wrapping_add(delay.max(0));
        while crate::kernel::VSync(-1).wrapping_sub(end) < 0 {
            core::hint::spin_loop();
        }
    }

    /// The seek bound of this try.
    pub fn seek_poll() -> super::WaitTime {
        // SAFETY: see the statics.
        unsafe {
            if ARMED && FAILS > 0 {
                FAILS -= 1;
                return super::WaitTime::from_clocks(1);
            }
            ARMED = false;
        }
        super::SEEK_POLL
    }
}

#[cfg(not(feature = "test-cd-fault"))]
mod fault {
    /// The seek bound of every try.
    #[inline(always)]
    pub fn seek_poll() -> super::WaitTime {
        super::SEEK_POLL
    }
}

/// The sector `lba`, in the shim's one sector buffer (kept while it holds `lba`; the next read may
/// replace it): what the layer's directory walk reads. `None` when the drive failed.
pub(crate) fn sector(lba: u32) -> Option<&'static [u32; SECTOR_WORDS]> {
    // SAFETY: see the statics.
    unsafe {
        if SCRATCH_LBA != lba {
            SCRATCH_LBA = u32::MAX;
            if !read(lba, 1, (&raw mut SCRATCH).cast(), false, None) {
                return None;
            }
            SCRATCH_LBA = lba;
            if lba == psx_io::iso::PVD_LBA {
                // the volume space size is at byte 80 of the descriptor
                let b = psx_io::iso::as_bytes(&*(&raw const SCRATCH));
                VOLUME_END = u32::from_le_bytes([b[80], b[81], b[82], b[83]]);
            }
        }
        Some(&*(&raw const SCRATCH))
    }
}

/// Forget the sector kept (a new disc session: `CdInit`).
pub(crate) fn forget() {
    // SAFETY: see the statics.
    unsafe { SCRATCH_LBA = u32::MAX };
}

// ------------------------------------------------------------------------------------- the ABI

/// `psx_core_read`: read `sectors` 2048-byte sectors from `lba` (the disc's logical sector; the core
/// will take member-relative ones) into `dst` (word aligned, whole sectors), blocking, with the
/// retries, ending paused. `hook(i)` runs after sector `i` is in RAM. 0, or `STAGE_READ_FAILED`.
///
/// RESTORE: PLATFORM. A one-sector request for the sector after the last read is answered from the
/// sector kept (`read_once`), with no drive start: NMS's .SPR tables are read so, 65 of 360X's
/// Mission 1's 135 starts.
#[no_mangle]
pub unsafe extern "C" fn core_read(lba: u32, sectors: u32, dst: *mut u32, hook: Option<extern "C" fn(u32)>) -> u32 {
    if sectors == 0 {
        return 0;
    }
    // SAFETY: see the statics.
    if sectors == 1 && unsafe { SCRATCH_LBA } == lba {
        // SAFETY: the caller's buffer holds a sector; SCRATCH is one.
        unsafe { core::ptr::copy_nonoverlapping((&raw const SCRATCH) as *const u32, dst, SECTOR_WORDS) };
        if let Some(hook) = hook {
            hook(0);
        }
        return 0;
    }
    if read(lba, sectors, dst, true, hook) { 0 } else { STAGE_READ_FAILED }
}

/// `psx_core_pause`: the drive settled and paused, proved (today: the reader's handshake, then its
/// Pause). 0, or `STAGE_READ_FAILED` when the drive did not answer.
#[no_mangle]
pub extern "C" fn core_pause() -> u32 {
    let ok = reader().prepare();
    reader().stop();
    if ok { 0 } else { STAGE_READ_FAILED }
}

/// `psx_core_client_attach`: a stream client (the film player) takes the drive. 0, or
/// `STAGE_CLIENT` when one already holds it.
#[no_mangle]
pub extern "C" fn core_client_attach() -> u32 {
    // SAFETY: see the statics.
    unsafe {
        if CLIENT {
            return STAGE_CLIENT;
        }
        CLIENT = true;
        SCRATCH_LBA = u32::MAX;
    }
    psx_core::link::client_attach()
}

/// `psx_core_client_detach`: the client gives the drive back, paused (the next read settles it
/// again in any case: every read starts with the reader's handshake).
#[no_mangle]
pub extern "C" fn core_client_detach() {
    // SAFETY: see the statics.
    unsafe {
        CLIENT = false;
        SCRATCH_LBA = u32::MAX;
    }
    psx_core::link::client_detach();
}

// ------------------------------------------------------------------------ the scene core (psx-core)

/// The program's RAM copy of the disc's scene table (CORE-ABI v2), read from LBA 22 by the core's
/// read the first time an image is not in RAM (`psx_core::link::load_image`); zeroed until then.
// SAFETY (all statics below): foreground only.
static mut TABLE: [u32; 16 + 8 * 62] = [0; 16 + 8 * 62];
/// Whether the scene core started this program (handoff flag bit 0), not a BIOS boot.
static mut ENTERED_BY_CORE: bool = false;
/// The images in RAM, one bit an image index: at boot the ones the EXE carries (the program's row says
/// the same to the core, tools/scenes.py `manifest`) and the boot code; then what the core loaded. The
/// layer asks the core only for an image not in it, so a game that never leaves its carried images
/// (QD Up, resident; 360X, MISSION resident and FRONT in the heap's bottom) never reads the table.
static mut IN_RAM: u32 = 0;

extern "C" {
    /// The image index the build gave the boot code (0: none), an absolute symbol (tools/scenes.py).
    static __scene_boot_id: u8;
    /// The images the EXE carries, one bit an image index (tools/scenes.py; psyq.ld provides 0).
    static __scene_preloaded: u8;
    /// 1 when the region images share one address (`arrange overlap`): a load replaces every other.
    static __scene_overlap: u8;
    /// The images that run at the heap's very bottom (tools/scenes.py `heap`), one bit an image index:
    /// the EXE carries them and the game never allocates that low, so they stay in RAM for good.
    static __scene_heap: u8;
}

fn bit(image: u32) -> u32 {
    if image == 0 { 0 } else { 1 << image }
}

/// Whether the scene core started this program: the boot splash is the disc's first program's alone,
/// so a program the core switched to does not play it again.
pub(crate) fn entered_by_core() -> bool {
    // SAFETY: see the statics.
    unsafe { ENTERED_BY_CORE }
}

/// Once, from the layer's `main` (the core's `__core_boot` and attach ran before `_start`): note how
/// the program was started and which images are in RAM. No disc read.
pub(crate) fn attach() {
    let flags = psx_core::link::block_word(psx_core::abi::block::OFF_FLAGS);
    // SAFETY: see the statics.
    unsafe {
        ENTERED_BY_CORE = psx_core::link::block_valid() && flags & psx_core::abi::block::FLAG_CORE_ENTRY != 0;
        // The boot code is in RAM however the program was started: the core's switch loads the program
        // image with the boot store in it, and `__psyq_boot_entry` moves it up on every entry. A core
        // entry skips only the splash (`PsxSplash`), not the boot image (the film player is in it).
        IN_RAM = &raw const __scene_preloaded as u32
            | &raw const __scene_heap as u32
            | bit(&raw const __scene_boot_id as u32);
    }
}

/// The scene the handoff block says is current.
#[no_mangle]
pub extern "C" fn core_current_scene() -> u32 {
    psx_core::link::block_word(0x14) & 0xFFFF
}

/// `psx_core_load_image(image_index, table)`: overlay `image` into its range unless it is in RAM, FNV
/// checked, I-cache flushed, the core's loaded mask updated, returning on the caller's stack. The table
/// argument is the layer's own copy (`TABLE`); the C declaration keeps the core's shape. The boot code
/// is not an image of the table: it is valid until the first other image is entered.
#[no_mangle]
pub extern "C" fn core_load_image(image: u32, _table: *const u32) -> u32 {
    let boot = &raw const __scene_boot_id as u32;
    // SAFETY: see the statics.
    unsafe {
        if image != boot {
            IN_RAM &= !bit(boot);
        }
        if IN_RAM & bit(image) != 0 {
            return 0;
        }
        if image == boot {
            return STAGE_NO_ROW;
        }
    }
    // SAFETY: see the statics; the core follows its ABI.
    let r = psx_core::link::load_image(image, unsafe { &mut *(&raw mut TABLE) });
    // The layer's reader settles the drive again before its next read in any case.
    forget();
    if r == 0 {
        // SAFETY: see the statics.
        unsafe {
            IN_RAM = if &raw const __scene_overlap as u32 != 0 { bit(image) } else { IN_RAM | bit(image) };
        }
    }
    r
}
