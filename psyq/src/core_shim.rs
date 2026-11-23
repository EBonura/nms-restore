//! RESTORE: PLATFORM. The one seam between the layer and the SDK's resident scene core.
//!
//! Every place the games touch the core goes through here: the drive (every sector the layer reads),
//! loading a scene image into its fixed range, the scene the handoff block reports, and a stream
//! client taking the drive (the film). The names and meanings follow the core's ABI (scene-system
//! CORE-ABI.md section 3: `psx_core_read`, `psx_core_pause`, `psx_core_client_attach` and
//! `_detach`); `core_load_image` is not in that ABI yet (it is the row kind 1 "Image" the table
//! already names) and is asked for. Declared for C in `psyq/include/psx_core_shim.h`.
//!
//! Until the core exists this file is the core: it drives the drive with the SDK's polled sector
//! reader, as the layer always has, and writes the handoff block itself. When the core lands, only
//! this file changes: each function becomes a `jal` to its slot, and `attach` goes (the core's
//! `__core_boot` does it before `_start`).
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
pub const STAGE_TABLE_MAGIC: u32 = 0x21;
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
    0
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
}

// ---------------------------------------------------------------------------- handoff block

/// The handoff block (CORE-ABI.md 2), at its fixed address above the core's window.
const BLOCK: usize = 0x801F_FF00;
const BLOCK_MAGIC: u32 = 0x4243_5350;
const OFF_CURRENT: usize = 0x14;
const OFF_PREVIOUS: usize = 0x16;
const OFF_COUNTER: usize = 0x1C;

fn block_u16(off: usize, v: u16) {
    // SAFETY: the handoff block, RAM above the program's stack that nothing else uses.
    unsafe { core::ptr::write_volatile((BLOCK + off) as *mut u16, v) };
}

fn block_u32(off: usize, v: u32) {
    // SAFETY: as block_u16.
    unsafe { core::ptr::write_volatile((BLOCK + off) as *mut u32, v) };
}

/// What the core's `__psx_core_attach` does on a cold entry (CORE-ABI.md 4.3, step 4): a fresh
/// block, the program's row (scene 0) current, no previous scene. Called once by the layer's
/// `main`; it goes when the core lands. `a0..a2` are psx-rt's entry registers (`disc_base`).
pub(crate) fn attach() {
    for off in (0..0x100).step_by(4) {
        block_u32(off, 0);
    }
    block_u32(0x00, BLOCK_MAGIC);
    block_u16(0x04, 1);
    block_u16(0x06, 0x100);
    block_u32(0x0C, psx_io::disc_base::lba_offset());
    block_u32(0x10, u32::from(psx_io::disc_base::cdda_track_base()));
    // The boot scene's image comes in with the program when the EXE carries it (tools/scenes.py `preload`).
    block_u16(OFF_CURRENT, &raw const __scene_preloaded as u16);
    block_u16(OFF_PREVIOUS, 0xFFFF);
    block_u32(0xFC, !BLOCK_MAGIC);
}

/// The scene the handoff block says is current (0 the program itself, before any image).
#[no_mangle]
pub extern "C" fn core_current_scene() -> u32 {
    // SAFETY: as block_u16.
    u32::from(unsafe { core::ptr::read_volatile((BLOCK + OFF_CURRENT) as *const u16) })
}

/// Record a switch to `scene` (the core does this at every switch).
fn entered(scene: u32) {
    block_u16(OFF_PREVIOUS, core_current_scene() as u16);
    block_u16(OFF_CURRENT, scene as u16);
    // SAFETY: as block_u16.
    let n = unsafe { core::ptr::read_volatile((BLOCK + OFF_COUNTER) as *const u32) };
    block_u32(OFF_COUNTER, n.wrapping_add(1));
}

// ------------------------------------------------------------------------------ scene images

/// What the shim keeps of a table row (CORE-ABI.md 1): its sectors and where they go.
#[derive(Clone, Copy)]
struct Row {
    lba: u32,
    sectors: u32,
    t_addr: u32,
    t_size: u32,
    fnv: u32,
    tries: u8,
}

const MAX_ROWS: usize = 8;
const NO_ROW: Row = Row { lba: 0, sectors: 0, t_addr: 0, t_size: 0, fnv: 0, tries: 0 };
// SAFETY (both): foreground-only statics.
static mut ROWS: [Row; MAX_ROWS] = [NO_ROW; MAX_ROWS];
/// The table's file has been read (its rows are in `ROWS`).
static mut TABLE_READ: bool = false;

extern "C" {
    /// The region the images share (tools/scenes.py's linker script; psyq.ld provides 0 for a
    /// program without images).
    static __scene_region_start: u8;
    static __scene_region_end: u8;
    /// The scene id of the image the EXE carries in the region (0: none), an absolute symbol.
    static __scene_preloaded: u8;
}

/// Word FNV-1a (CORE-ABI.md: offset basis 0x811C9DC5, prime 0x01000193, a word at a step).
fn word_fnv(words: &[u32]) -> u32 {
    let mut h: u32 = 0x811C_9DC5;
    for &w in words {
        h = (h ^ w).wrapping_mul(0x0100_0193);
    }
    h
}

/// Read the scene table: the first sector of `\SCENES.IMG`, whose image rows count their LBAs from
/// the file's start (tools/scenes.py; the core's table, at a fixed LBA, will carry disc LBAs).
fn read_table() -> u32 {
    let Some((base, _)) = crate::cd::find_file(b"\\SCENES.IMG;1") else {
        return STAGE_READ_FAILED;
    };
    let Some(sector) = sector(base) else {
        return STAGE_READ_FAILED;
    };
    if sector[0] != u32::from_le_bytes(*b"PSXS") || sector[1] != u32::from_le_bytes(*b"CN01") {
        return STAGE_TABLE_MAGIC;
    }
    let count = (sector[2] >> 16) as usize;
    let row_bytes = (sector[3] & 0xFFFF) as usize;
    if row_bytes != 64 || count > 31 || word_fnv(&sector[16..16 + count * 16]) != sector[5] {
        return STAGE_TABLE_MAGIC;
    }
    for id in 0..count.min(MAX_ROWS) {
        let r = &sector[16 + id * 16..32 + id * 16];
        if r[0] & 0xFFFF != id as u32 {
            continue;
        }
        let row = Row { lba: base + r[1], sectors: r[2], t_addr: r[3], t_size: r[4], fnv: r[6], tries: (r[9] >> 16) as u8 };
        // SAFETY: see the statics.
        unsafe { ROWS[id] = if (r[0] >> 16) & 0xFF == 1 { row } else { NO_ROW } };
    }
    // SAFETY: see the statics.
    unsafe { TABLE_READ = true };
    0
}

/// Load scene image `scene` (a kind 1 row) into its fixed range while the program keeps running:
/// read its sectors, check its word FNV (a bad one is read again, up to the row's attempts), flush
/// the instruction cache, and record the switch in the handoff block. 0, or a stage code (nothing
/// is run from the range then). Not in the core's ABI yet: asked for.
#[no_mangle]
pub extern "C" fn core_load_image(scene: u32) -> u32 {
    // SAFETY: see the statics.
    if unsafe { CLIENT } {
        return STAGE_CLIENT;
    }
    // SAFETY: see the statics.
    if !unsafe { TABLE_READ } {
        let r = read_table();
        if r != 0 {
            return r;
        }
    }
    // SAFETY: see the statics.
    let Some(row) = (unsafe { ROWS }).get(scene as usize).copied().filter(|r| r.sectors != 0) else {
        return STAGE_NO_ROW;
    };
    let (lo, hi) = (&raw const __scene_region_start as u32, &raw const __scene_region_end as u32);
    if row.t_addr < lo || row.t_size != row.sectors * 2048 || row.t_addr + row.t_size > hi {
        return STAGE_BOUNDS;
    }
    let dst = row.t_addr as *mut u32;
    let mut result = STAGE_CHECKSUM;
    for _ in 0..row.tries.max(1) {
        if !read(row.lba, row.sectors, dst, false, None) {
            result = STAGE_READ_FAILED;
            break;
        }
        // SAFETY: the range was just read, inside the region.
        if word_fnv(unsafe { core::slice::from_raw_parts(dst, row.t_size as usize / 4) }) == row.fnv {
            result = 0;
            break;
        }
    }
    if result != 0 {
        return result;
    }
    psx_rt::cache::flush_instruction_cache();
    entered(scene);
    0
}
