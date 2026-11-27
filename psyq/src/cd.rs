//! libcd: reading files from the game's CD.
//!
//! The calls NMS's loaders make (360X's `FILES.C` is the pattern, QD Up's
//! disc build uses the same): `CdInit`, `CdSearchFile` to find a file by its
//! ISO 9660 path, `CdControlB(CdlSeekL, &pos)` to say where to read,
//! `CdRead(sectors, buffer, mode)` and `CdReadSync(0, result)` to wait for it.
//!
//! Every read here is blocking: `CdRead` returns once the sectors are in
//! the buffer, so `CdReadSync` always finds the read finished. Every sector is
//! read through the scene core's seam (`core_shim.rs`, RESTORE: PLATFORM), which today drives the drive with the PSoXide SDK's polled sector reader
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
//! there is no SDK twin to call. Second (now in `core_shim.rs`, below the seam), the read-ahead cache:
//! NMS's loaders ask for the next file at the sector the last read ended at, and a drive start costs 40 ms
//! against a sector's 6.7 ms; the cache takes Mission 1 from 135 drive starts to 63. The SDK's
//! `psx-cdstream` is a streaming reader (reads queued ahead of the caller) and these reads block, so it does
//! not fit; `SectorReader::find` / `iso::find_in_root` walk the root only, where `CdSearchFile` takes
//! `\DIR\FILE` paths, so the per-component walk through `find_in_directory` stays.

use psx_io::cd::reader::SECTOR_WORDS;
use crate::core_shim;
use crate::path;
use psx_io::iso;

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
// SAFETY (all statics below): one thread of control, and none of them is
// touched from the VBlank handler.
/// Where the next `CdRead` starts (logical sector), set by Setloc / SeekL.
static mut TARGET: u32 = 0;
/// Whether the last `CdRead` delivered every sector.
static mut LAST_OK: bool = true;
static mut READY_CALLBACK: usize = 0;
/// The root directory's extent, once the volume descriptor has been read.
static mut ROOT: Option<iso::Extent> = None;

fn bcd(v: u8) -> u8 {
    (v >> 4) * 10 + (v & 15)
}

fn to_bcd(v: u32) -> u8 {
    ((v / 10) << 4) as u8 | (v % 10) as u8
}

/// The directory `dir`'s entry called `name`: each of its sectors is read (kept in `SCRATCH`) and
/// asked `psx_io::iso::find_in_directory`.
fn lookup(dir: iso::Extent, name: &[u8]) -> Option<iso::Extent> {
    let mut lba = dir.0;
    for _ in 0..iso::sectors(dir.1) {
        let sector: &[u32; SECTOR_WORDS] = core_shim::sector(lba)?;
        if let Some(entry) = iso::find_in_directory(iso::as_bytes(sector), name) {
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
    let root = iso::root_directory(iso::as_bytes(core_shim::sector(iso::PVD_LBA)?))?;
    // SAFETY: see the statics.
    unsafe { ROOT = Some(root) };
    Some(root)
}

/// Find a file by path: (first sector, size in bytes). Names compare without regard to case,
/// and the `;1` version of a name is optional.
pub(crate) fn find_file(path: &[u8]) -> Option<(u32, u32)> {
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
    let ok = core_shim::core_pause() == 0;
    // SAFETY: see the statics.
    unsafe { ROOT = None };
    core_shim::forget();
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
    // RESTORE: PLATFORM. Through the scene core's seam (core_shim.rs), which also answers a
    // request for sectors the last read ran on for with no drive start (docs/DECISIONS.md, 360X).
    // SAFETY: the caller's buffer holds `sectors` whole sectors, word aligned.
    let ok = unsafe { core_shim::core_read(lba, sectors as u32, buf, None) } == 0;
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
