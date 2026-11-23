//! libcd: reading files from the game's CD.
//!
//! The calls NMS's loaders make (360X's `FILES.C` is the pattern, QD Up's
//! disc build uses the same): `CdInit`, `CdSearchFile` to find a file by its
//! ISO 9660 path, `CdControlB(CdlSeekL, &pos)` to say where to read,
//! `CdRead(sectors, buffer, mode)` and `CdReadSync(0, result)` to wait for it.
//!
//! Every read here is blocking: `CdRead` returns once the sectors are in
//! the buffer, so `CdReadSync` always finds the read finished. The drive is
//! driven by the PSoXide SDK's polled sector reader
//! (`psx_io::cd::reader::SectorReader`): each `CdRead` is one SetLoc, SeekL,
//! SetMode, ReadN bracket (the sequence the BIOS uses), the sectors popped
//! from the data FIFO by the CPU, then a Pause. While a read runs only the
//! VBlank interrupt reaches the CPU, so the layer's VBlank work (controller,
//! VSync callback) carries on.
//!
//! `CdSearchFile` reads the volume descriptor at sector 16 and walks the
//! directories from the root, one component of the path at a time; names
//! compare without regard to case, and the `;1` version is optional.
//! Positions (`CdlLOC`) are minute, second, sector in BCD counted from the
//! start of the disc, so sector 0 of the data track is 00:02:00
//! (`CdIntToPos`, `CdPosToInt`).
//!
//! What is the SDK's and what stays the layer's (closed by measurement, wave 4; do not reopen without a new
//! number): the drive is `psx_io::cd::reader::SectorReader` and the lookups are `psx_io::iso`
//! (`root_directory`, `find_in_directory`, `PVD_LBA`), the SDK's own code. Two things stay here. First, the
//! libcd shim itself (BCD `CdlLOC` positions, `CdControl` commands that only set where the next read starts,
//! the blocking `CdRead` with an always-finished `CdReadSync`): it is the 1996 API the games' C calls, so
//! there is no SDK twin to call. Second, the one-sector read-ahead cache (`SCRATCH`, `read_sectors`' `ahead`):
//! NMS's loaders ask for the next file at the sector the last read ended at, and a drive start costs 40 ms
//! against a sector's 6.7 ms; the cache answers 65 of Mission 1's 135 drive starts with no seek. The SDK's
//! `psx-cdstream` is a streaming reader (reads queued ahead of the caller) and these reads block, so it does
//! not fit; `SectorReader::find` / `iso::find_in_root` walk the root only, where `CdSearchFile` takes
//! `\DIR\FILE` paths, so the per-component walk through `find_in_directory` stays.

use psx_io::cd::reader::{SectorReader, SECTOR_WORDS};
use psx_io::cd::wait::{WaitTime, FIRST_SECTOR_WAIT};
use crate::path;
use psx_io::iso;
use psx_io::periph::Cd;

/// Disc position, BCD minute / second / sector (the drive's own format).
#[repr(C)]
#[derive(Clone, Copy)]
pub struct CdlLOC {
    pub minute: u8,
    pub second: u8,
    pub sector: u8,
    pub track: u8,
}

/// A file found by `CdSearchFile`.
#[repr(C)]
pub struct CdlFILE {
    pub pos: CdlLOC,
    pub size: u32,
    pub name: [u8; 16],
}

// CD-ROM controller command numbers (the drive's), as `CdControl` takes them.
const CDL_NOP: u8 = 0x01;
const CDL_SETLOC: u8 = 0x02;
const CDL_PAUSE: u8 = 0x09;
const CDL_STOP: u8 = 0x08;
const CDL_SETMODE: u8 = 0x0E;
const CDL_SEEKL: u8 = 0x15;
const CDL_SEEKP: u8 = 0x16;

/// The drive's status byte with the motor on and nothing else to report.
const STATUS_MOTOR_ON: u8 = 0x02;
/// Bound on the SeekL wait (a full-stroke seek at double speed fits): the SDK's own bound for a seek or a
/// read's first sector, in time (`psx_io::cd::wait`), where the layer had a spin count.
const SEEK_POLL: WaitTime = FIRST_SECTOR_WAIT;
/// Sector of the ISO 9660 primary volume descriptor.

// SAFETY (all statics below): one thread of control, and none of them is
// touched from the VBlank handler.
static mut READER: SectorReader = SectorReader::with_cd(unsafe { Cd::steal() });
/// Where the next `CdRead` starts (logical sector), set by Setloc / SeekL.
static mut TARGET: u32 = 0;
/// Whether the last `CdRead` delivered every sector.
static mut LAST_OK: bool = true;
static mut READY_CALLBACK: usize = 0;
/// One sector for directory reads.
static mut SCRATCH: [u32; SECTOR_WORDS] = [0; SECTOR_WORDS];
/// The sector `SCRATCH` holds (`u32::MAX`: none). A game that looks up its
/// files one by one reads the same directory sector each time; kept, it
/// costs one seek instead of two per file.
static mut SCRATCH_LBA: u32 = u32::MAX;
/// The root directory's extent, once the volume descriptor has been read.
static mut ROOT: Option<iso::Extent> = None;
/// The disc's size in sectors, from the volume descriptor (0 until it has been read).
/// The sector kept after a read (`read_sectors`) is only read when it is on the disc.
static mut VOLUME_END: u32 = 0;

fn reader() -> &'static mut SectorReader {
    // SAFETY: see the statics.
    unsafe { &mut *(&raw mut READER) }
}

fn bcd(v: u8) -> u8 {
    (v >> 4) * 10 + (v & 15)
}

fn to_bcd(v: u32) -> u8 {
    ((v / 10) << 4) as u8 | (v % 10) as u8
}

/// Tries a read gets: the first and four more, each after a full re-sync of the drive.
///
/// RESTORE: PLATFORM. On a console a read can fail where the emulator never fails one: after the
/// drive has idled (the console measured 4.4 s to the first sector after a Pause and 15 s idle,
/// silicon v2.7) a seek or the first sector can take longer than the SDK reader's spin-count
/// bounds. A failed read was handed to the game as a failure, which neither game checked. Now
/// it is tried again (`resync`), and only after the last try is it the caller's failure; the
/// games then show which file, step and error (`loadfail.rs`).
const READ_TRIES: u32 = 5;

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

// SAFETY: as the statics above.
static mut LAST_FAILURE: Option<Failure> = None;

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

/// One try of `read_sectors`: the step and sector it stopped at when it fails.
fn read_once(lba: u32, count: u32, dst: *mut u32, ahead: bool) -> Result<(), (u8, u32)> {
    let rd = reader();
    if !rd.prepare() {
        return Err((STEP_PREPARE, lba));
    }
    if !rd.start_read_seek_first(lba, fault::seek_poll()) {
        return Err((STEP_SEEK, lba));
    }
    for i in 0..count as usize {
        // SAFETY: the caller's buffer holds `count` whole sectors and is word
        // aligned (CdRead takes a `u_long *`).
        let sector = unsafe { &mut *(dst.add(i * SECTOR_WORDS) as *mut [u32; SECTOR_WORDS]) };
        if !rd.read_sector(sector) {
            return Err((STEP_SECTOR, lba + i as u32));
        }
    }
    // SAFETY: see the statics.
    if ahead && lba + count < unsafe { VOLUME_END } {
        // RESTORE: PLATFORM. A game that loads its files one after another asks for the next
        // one at the sector this read ends at, and starting a read costs 40 ms to a
        // sector's 6.7 ms: the stream runs on for that one sector, if the disc has it (a read
        // past the last sector of the image answers with an error or nothing, and the layer
        // would wait for it).
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
/// one sector after them, which goes into `SCRATCH` (see `CdRead`).
fn read_sectors(lba: u32, count: u32, dst: *mut u32, ahead: bool) -> bool {
    for tries in 1..=READ_TRIES {
        match read_once(lba, count, dst, ahead) {
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

fn scratch() -> &'static mut [u32; SECTOR_WORDS] {
    // SAFETY: see the statics.
    unsafe { &mut *(&raw mut SCRATCH) }
}

/// The sector read for `psx_io::iso::find`: `SCRATCH` itself, kept when it already holds `lba`.
fn read_cached(lba: u32, into: &mut [u32; SECTOR_WORDS]) -> bool {
    // SAFETY: see the statics.
    unsafe {
        if SCRATCH_LBA == lba {
            return true;
        }
        SCRATCH_LBA = u32::MAX;
        if !read_sectors(lba, 1, into.as_mut_ptr(), false) {
            return false;
        }
        SCRATCH_LBA = lba;
        if lba == iso::PVD_LBA {
            // the volume space size is at byte 80 of the descriptor
            let b = iso::as_bytes(into);
            VOLUME_END = u32::from_le_bytes([b[80], b[81], b[82], b[83]]);
        }
    }
    true
}

/// The directory `dir`'s entry called `name`: each of its sectors is read (kept in `SCRATCH`) and
/// asked `psx_io::iso::find_in_directory`.
fn lookup(dir: iso::Extent, name: &[u8]) -> Option<iso::Extent> {
    let mut lba = dir.0;
    for _ in 0..iso::sectors(dir.1) {
        if !read_cached(lba, scratch()) {
            return None;
        }
        if let Some(entry) = iso::find_in_directory(iso::as_bytes(scratch()), name) {
            return Some(entry);
        }
        lba += 1;
    }
    None
}

/// The root directory, from the volume descriptor (read once).
fn root() -> Option<iso::Extent> {
    // SAFETY: see the statics.
    if let Some(root) = unsafe { ROOT } {
        return Some(root);
    }
    if !read_cached(iso::PVD_LBA, scratch()) {
        return None;
    }
    let root = iso::root_directory(iso::as_bytes(scratch()))?;
    // SAFETY: see the statics.
    unsafe { ROOT = Some(root) };
    Some(root)
}

/// Find a file by path: (first sector, size in bytes). Names compare without regard to case,
/// and the `;1` version of a name is optional.
fn find_file(path: &[u8]) -> Option<(u32, u32)> {
    let mut text = [0u8; path::MAX_PATH];
    let (names, count) = path::split(path, &mut text)?;
    let mut entry = root()?;
    for name in &names[..count] {
        entry = lookup(entry, name)?;
    }
    Some(entry)
}

// -------------------------------------------------------------------- API

/// Get the drive ready for reads. 1 when it answered.
#[no_mangle]
pub extern "C" fn CdInit() -> i32 {
    let ok = reader().prepare();
    // SAFETY: see the statics.
    unsafe {
        ROOT = None;
        SCRATCH_LBA = u32::MAX;
    }
    reader().stop();
    ok as i32
}

/// Logical sector to disc position (sector 0 is 00:02:00).
#[no_mangle]
pub unsafe extern "C" fn CdIntToPos(i: i32, p: *mut CdlLOC) -> *mut CdlLOC {
    let a = (i + 150) as u32;
    // SAFETY: the caller's position record.
    unsafe {
        (*p).minute = to_bcd(a / (60 * 75));
        (*p).second = to_bcd((a / 75) % 60);
        (*p).sector = to_bcd(a % 75);
        (*p).track = 0;
    }
    p
}

/// Disc position to logical sector.
#[no_mangle]
pub unsafe extern "C" fn CdPosToInt(p: *const CdlLOC) -> i32 {
    // SAFETY: the caller's position record.
    let p = unsafe { &*p };
    (bcd(p.minute) as i32 * 60 + bcd(p.second) as i32) * 75 + bcd(p.sector) as i32 - 150
}

/// Find `name` (`\DIR\FILE.EXT;1`) on the disc and fill `fp` with its
/// position, size and name. Returns `fp`, or 0 when there is no such file.
#[no_mangle]
pub unsafe extern "C" fn CdSearchFile(fp: *mut CdlFILE, name: *const u8) -> *mut CdlFILE {
    // SAFETY: a NUL-terminated C string from the game.
    let path = unsafe { core::ffi::CStr::from_ptr(name.cast()) }.to_bytes();
    let Some((lba, size)) = find_file(path) else {
        return core::ptr::null_mut();
    };
    // SAFETY: the caller's file record.
    unsafe {
        CdIntToPos(lba as i32, &raw mut (*fp).pos);
        (*fp).size = size;
        let base = path.rsplit(|&c| c == b'\\' || c == b'/').next().unwrap_or(path);
        let n = base.len().min(15);
        let mut rec = [0u8; 16];
        rec[..n].copy_from_slice(&base[..n]);
        (*fp).name = rec;
    }
    fp
}

/// Primitive drive commands. Setloc and SeekL/SeekP set where the next
/// `CdRead` starts (the seek itself happens there); Setmode, Pause, Stop
/// and Nop have nothing left to do, since every read ends paused. 1 = done.
#[no_mangle]
pub unsafe extern "C" fn CdControl(com: u8, param: *const u8, result: *mut u8) -> i32 {
    match com {
        CDL_SETLOC | CDL_SEEKL | CDL_SEEKP => {
            if !param.is_null() {
                // SAFETY: the game's CdlLOC.
                unsafe { TARGET = CdPosToInt(param as *const CdlLOC) as u32 };
            }
        }
        CDL_SETMODE | CDL_PAUSE | CDL_STOP | CDL_NOP => {}
        _ => stub!("CdControl (command not handled)"),
    }
    if !result.is_null() {
        // SAFETY: the game's result buffer (8 bytes in Psy-Q's convention).
        unsafe { *result = STATUS_MOTOR_ON };
    }
    1
}

/// `CdControl`, waiting for the command to finish; the same here.
#[no_mangle]
pub unsafe extern "C" fn CdControlB(com: u8, param: *const u8, result: *mut u8) -> i32 {
    // SAFETY: as CdControl.
    unsafe { CdControl(com, param, result) }
}

/// Read `sectors` 2048-byte sectors from the position set last into `buf`
/// (word aligned, room for whole sectors). Returns once they are there: 1,
/// or 0 when the drive failed. The position moves on past them.
#[no_mangle]
pub unsafe extern "C" fn CdRead(sectors: i32, buf: *mut u32, _mode: i32) -> i32 {
    if sectors <= 0 {
        return 1;
    }
    // SAFETY: see the statics.
    let lba = unsafe { TARGET };
    // RESTORE: PLATFORM. The sector after the last read is kept (`read_sectors`), and a
    // one-sector request for it, as NMS's .SPR tables are, is answered from there with
    // no drive start (no seek, no first-sector wait, no Pause): 65 of Mission 1's
    // 135 starts. docs/DECISIONS.md, 360X.
    if sectors == 1 && unsafe { SCRATCH_LBA } == lba {
        // SAFETY: the caller's buffer holds a sector; SCRATCH is one.
        unsafe {
            core::ptr::copy_nonoverlapping((&raw const SCRATCH) as *const u32, buf, SECTOR_WORDS);
            LAST_OK = true;
            TARGET = lba + 1;
        }
        return 1;
    }
    let ok = read_sectors(lba, sectors as u32, buf, true);
    unsafe {
        LAST_OK = ok;
        TARGET = lba + sectors as u32;
    }
    ok as i32
}

/// Wait for the read `CdRead` started: 0 = finished, -1 = it failed (reads
/// are blocking, so there is never anything left to wait for).
#[no_mangle]
pub unsafe extern "C" fn CdReadSync(_mode: i32, result: *mut u8) -> i32 {
    if !result.is_null() {
        // SAFETY: the game's result buffer.
        unsafe { *result = STATUS_MOTOR_ON };
    }
    // SAFETY: see the statics.
    if unsafe { LAST_OK } { 0 } else { -1 }
}

/// Wait for a `CdControl` command: always complete (2, `CdlComplete`).
#[no_mangle]
pub unsafe extern "C" fn CdSync(_mode: i32, result: *mut u8) -> i32 {
    if !result.is_null() {
        // SAFETY: the game's result buffer.
        unsafe { *result = STATUS_MOTOR_ON };
    }
    2
}

/// Set the function called when a read finishes; returns the previous one.
/// Kept, never called: `CdRead` has finished by the time it returns.
#[no_mangle]
pub extern "C" fn CdReadyCallback(func: usize) -> usize {
    // SAFETY: see the statics.
    unsafe { core::mem::replace(&mut *(&raw mut READY_CALLBACK), func) }
}
