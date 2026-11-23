//! The memory card: the card driver calls (`_card_info`, `_card_load`,
//! `_card_read`, `_card_write`, `_card_clear`) and the BIOS file calls on
//! "bu00:" (port 1) and "bu10:" (port 2): `open`, `read`, `write`, `close`,
//! `format`, `delete`.
//!
//! Every call talks to the card over SIO0 through psx-mc's `HardwareCard`
//! (one 128-byte frame per transaction) and finishes before it returns. The
//! controller poll the VBlank handler does (kernel.rs) is paused for the
//! duration, since both use the same port.
//!
//! Driver calls report as the BIOS did, by delivering an event the game
//! waits for with `TestEvent`: `_card_info` and `_card_load` on `SwCARD`,
//! `_card_read`, `_card_write` and `_card_clear` on `HwCARD`. The spec is
//! `EvSpIOE` when the call worked (for `_card_load`: the card is formatted),
//! `EvSpNEW` when `_card_load` finds no directory (unformatted), `EvSpTIMOUT`
//! when no card answers, `EvSpERROR` when one answers wrongly. Since the
//! event is delivered before the call returns, a game that clears the events
//! first and then calls (as 360X's LOADSAVE.C does) sees the answer at once.
//!
//! Files use the card's standard directory (frame 0 "MC", frames 1 to 15
//! one entry per 8 KiB block: state 51h first, 52h middle, 53h last, A0h
//! free; size; link to the next entry; name; XOR checksum), the layout
//! psx-mc's `Card` also writes, so a card made here reads in the console's
//! card manager and in psx-mc. A file's bytes are written as the game gives
//! them: the game writes its own "SC" title frame and icon at the start, as
//! on the console. `open` with `O_CREAT` makes a file of `mode >> 16` blocks
//! (at least one) and fails if one of that name exists; names compare as the
//! BIOS's did, '?' matching any one character and '*' the rest of the name.
//! Reads and writes go from the position the last one left, in whole frames
//! or not (a partial frame is read, changed and written back).

use crate::kernel::{self, deliver_event};
use psx_io::periph::ControllerPort;
use psx_mc::{Block, Card, Error, HardwareCard, Slot, FRAME_SIZE};

const SW_CARD: u32 = 0xF400_0001;
const HW_CARD: u32 = 0xF000_0011;
const EV_SP_IOE: u32 = 0x0004;
const EV_SP_TIMOUT: u32 = 0x0100;
const EV_SP_NEW: u32 = 0x2000;
const EV_SP_ERROR: u32 = 0x8000;

const O_CREAT: i32 = 0x0200;

/// Directory entry offsets and states (frames 1 to 15 of block 0): psx-mc's, the layout both write.
use psx_mc::dir::{
    checksum_inline as checksum, chain_in, find_in, is_free, read_into, E_CHK, E_LINK, E_NAME, E_SIZE, E_STATE, LINK_NONE,
    ST_FIRST, ST_FREE, ST_LAST, ST_MIDDLE,
};
use psx_mc::DirOptions;
const NAME_LEN: usize = 20;
const BLOCKS: usize = 15;
const BLOCK_BYTES: usize = 8192;
const FRAMES_PER_BLOCK: u16 = 64;

type Frame = [u8; FRAME_SIZE];

/// Run `f` on the card in `slot` with the VBlank controller poll paused.
fn with_card<R>(slot: Slot, f: impl FnOnce(&mut HardwareCard) -> R) -> R {
    let was = kernel::pause_pad();
    // SAFETY: the token only guards against two drivers using the port at
    // once; the one other user, the VBlank poll, is paused until we are done.
    let mut card = HardwareCard::on_port(unsafe { ControllerPort::steal() }, slot);
    let r = f(&mut card);
    kernel::resume_pad(was);
    r
}

/// A driver channel (0x00 port 1, 0x10 port 2) as a slot.
fn chan_slot(chan: i32) -> Slot {
    if chan & 0x10 != 0 { Slot::Two } else { Slot::One }
}

fn spec_of(r: Result<(), Error>) -> u32 {
    match r {
        Ok(()) => EV_SP_IOE,
        Err(Error::NoCard) => EV_SP_TIMOUT,
        Err(_) => EV_SP_ERROR,
    }
}

// ------------------------------------------------------------ driver calls

#[no_mangle]
pub extern "C" fn InitCARD(_pad_enable: i32) {}

#[no_mangle]
pub extern "C" fn StartCARD() -> i32 {
    1
}

#[no_mangle]
pub extern "C" fn StopCARD() -> i32 {
    1
}

#[no_mangle]
pub extern "C" fn _bu_init() {}

/// Is a card there: `EvSpIOE`, else `EvSpTIMOUT` (none) or `EvSpERROR`.
#[no_mangle]
pub extern "C" fn _card_info(chan: i32) -> i32 {
    let mut f = [0; FRAME_SIZE];
    let r = with_card(chan_slot(chan), |c| c.read_frame(0, &mut f));
    deliver_event(SW_CARD, spec_of(r));
    1
}

/// Read the directory header: `EvSpIOE` formatted, `EvSpNEW` not.
#[no_mangle]
pub extern "C" fn _card_load(chan: i32) -> i32 {
    let mut f = [0; FRAME_SIZE];
    let r = with_card(chan_slot(chan), |c| c.read_frame(0, &mut f));
    let spec = match r {
        Ok(()) if f[0] == b'M' && f[1] == b'C' => EV_SP_IOE,
        Ok(()) => EV_SP_NEW,
        e => spec_of(e),
    };
    deliver_event(SW_CARD, spec);
    1
}

/// Frame (sector) `block` of the card into `buf`, 128 bytes.
#[no_mangle]
pub unsafe extern "C" fn _card_read(chan: i32, block: i32, buf: *mut u8) -> i32 {
    let mut f = [0; FRAME_SIZE];
    let r = with_card(chan_slot(chan), |c| c.read_frame(block as u16, &mut f));
    if r.is_ok() {
        // SAFETY: the caller's buffer holds a frame, as on the console.
        unsafe { core::ptr::copy_nonoverlapping(f.as_ptr(), buf, FRAME_SIZE) };
    }
    deliver_event(HW_CARD, spec_of(r));
    1
}

/// 128 bytes from `buf` to frame (sector) `block` of the card.
#[no_mangle]
pub unsafe extern "C" fn _card_write(chan: i32, block: i32, buf: *mut u8) -> i32 {
    let mut f = [0; FRAME_SIZE];
    // SAFETY: as _card_read.
    unsafe { core::ptr::copy_nonoverlapping(buf, f.as_mut_ptr(), FRAME_SIZE) };
    let r = with_card(chan_slot(chan), |c| c.write_frame(block as u16, &f));
    deliver_event(HW_CARD, spec_of(r));
    1
}

/// On the console a dummy write that clears the card's "new card" flag;
/// this layer never reports a new card, so it only checks one is there.
#[no_mangle]
pub extern "C" fn _card_clear(chan: i32) -> i32 {
    let mut f = [0; FRAME_SIZE];
    let r = with_card(chan_slot(chan), |c| c.read_frame(0, &mut f));
    deliver_event(HW_CARD, spec_of(r));
    1
}

#[no_mangle]
pub extern "C" fn _new_card() {}

// ---------------------------------------------------------- file system

/// "bu00:NAME" / "bu10:NAME": the slot and the name's bytes.
unsafe fn parse(path: *const u8) -> Option<(Slot, &'static [u8])> {
    if path.is_null() {
        return None;
    }
    // SAFETY: a NUL-terminated C string from the game.
    let s = unsafe { core::ffi::CStr::from_ptr(path.cast()) }.to_bytes();
    let rest = s.strip_prefix(b"bu")?;
    let (dev, name) = (rest.get(..3)?, &rest[3..]);
    if dev[2] != b':' || dev[1] != b'0' {
        return None;
    }
    let slot = match dev[0] {
        b'0' => Slot::One,
        b'1' => Slot::Two,
        _ => return None,
    };
    Some((slot, name))
}

fn write_entry(card: &mut HardwareCard, i: usize, e: &mut Frame) -> Result<(), Error> {
    e[E_CHK] = checksum(e);
    card.write_frame(1 + i as u16, e)
}

/// The directory as the BIOS reads it: a bad checksum accepted, `?` and `*` in a name, a chain
/// that stops at a bad link (psx-mc's `DirOptions::BIOS`).
const BIOS: DirOptions = DirOptions::BIOS;

#[derive(Clone, Copy)]
struct File {
    open: bool,
    slot: Slot,
    blocks: [u8; BLOCKS],
    nblocks: usize,
    pos: usize,
}

const MAX_FILES: usize = 4;
const CLOSED: File = File { open: false, slot: Slot::One, blocks: [0; BLOCKS], nblocks: 0, pos: 0 };
static mut FILES: [File; MAX_FILES] = [CLOSED; MAX_FILES];

fn files() -> &'static mut [File; MAX_FILES] {
    // SAFETY: only these foreground calls touch the table, one at a time.
    unsafe { &mut *(&raw mut FILES) }
}

fn new_fd(slot: Slot, blocks: [u8; BLOCKS], nblocks: usize) -> i32 {
    for (fd, f) in files().iter_mut().enumerate() {
        if !f.open {
            *f = File { open: true, slot, blocks, nblocks, pos: 0 };
            return fd as i32;
        }
    }
    -1
}

/// Make `name` with `n` blocks: the entries, first to last, linked.
fn create(card: &mut HardwareCard, dir: &[Frame; BLOCKS], name: &[u8], n: usize) -> Result<[u8; BLOCKS], Error> {
    let mut free = [0usize; BLOCKS];
    let mut k = 0;
    for (i, e) in dir.iter().enumerate() {
        if is_free(e[E_STATE]) && k < n {
            free[k] = i;
            k += 1;
        }
    }
    if k < n {
        return Err(Error::NoSpace);
    }
    let mut blocks = [0; BLOCKS];
    for j in 0..n {
        let mut e = [0; FRAME_SIZE];
        e[E_STATE] = if j == 0 { ST_FIRST } else if j == n - 1 { ST_LAST } else { ST_MIDDLE };
        if j == 0 {
            e[E_SIZE..E_SIZE + 4].copy_from_slice(&((n * BLOCK_BYTES) as u32).to_le_bytes());
            let len = name.len().min(NAME_LEN);
            e[E_NAME..E_NAME + len].copy_from_slice(&name[..len]);
        }
        let link = if j + 1 < n { free[j + 1] as u16 } else { LINK_NONE };
        e[E_LINK..E_LINK + 2].copy_from_slice(&link.to_le_bytes());
        write_entry(card, free[j], &mut e)?;
        blocks[j] = free[j] as u8 + 1;
    }
    Ok(blocks)
}

/// Open (or with `O_CREAT`, make) a file on a card: a descriptor, or -1.
#[no_mangle]
pub unsafe extern "C" fn open(path: *const u8, mode: i32) -> i32 {
    // SAFETY: a C string from the game.
    let Some((slot, name)) = (unsafe { parse(path) }) else {
        return -1;
    };
    with_card(slot, |card| {
        let mut dir = [[0; FRAME_SIZE]; BLOCKS];
        if read_into(card, BIOS, &mut dir).is_err() {
            return -1;
        }
        if mode & O_CREAT != 0 {
            if name.is_empty() || name.len() > NAME_LEN || find_in(&dir, name, BIOS).is_some() {
                return -1;
            }
            let n = ((mode as u32 >> 16) as usize).clamp(1, BLOCKS);
            match create(card, &dir, name, n) {
                Ok(blocks) => new_fd(slot, blocks, n),
                Err(_) => -1,
            }
        } else {
            match find_in(&dir, name, BIOS) {
                Some(first) => {
                    let c = chain_in(&dir, first);
                    let (blocks, n) = (c.blocks, c.len);
                    new_fd(slot, blocks, n)
                }
                None => -1,
            }
        }
    })
}

fn file(fd: i32) -> Option<&'static mut File> {
    files().get_mut(usize::try_from(fd).ok()?).filter(|f| f.open)
}

/// Copy `n` bytes between the file (from its position) and `buf`, frame by
/// frame; `write` says which way. The count moved, or -1.
fn transfer(fd: i32, buf: *mut u8, n: i32, write: bool) -> i32 {
    let Some(f) = file(fd) else {
        return -1;
    };
    let size = f.nblocks * BLOCK_BYTES;
    let n = (n.max(0) as usize).min(size - f.pos.min(size));
    let (slot, blocks, start) = (f.slot, f.blocks, f.pos);
    let moved = with_card(slot, |card| {
        let mut done = 0;
        while done < n {
            let at = start + done;
            let frame = blocks[at / BLOCK_BYTES] as u16 * FRAMES_PER_BLOCK + ((at % BLOCK_BYTES) / FRAME_SIZE) as u16;
            let off = at % FRAME_SIZE;
            let len = (FRAME_SIZE - off).min(n - done);
            let mut fr = [0; FRAME_SIZE];
            if (!write || len < FRAME_SIZE) && card.read_frame(frame, &mut fr).is_err() {
                break;
            }
            // SAFETY: the caller's buffer holds n bytes.
            unsafe {
                if write {
                    core::ptr::copy_nonoverlapping(buf.add(done), fr.as_mut_ptr().add(off), len);
                } else {
                    core::ptr::copy_nonoverlapping(fr.as_ptr().add(off), buf.add(done), len);
                }
            }
            if write && card.write_frame(frame, &fr).is_err() {
                break;
            }
            done += len;
        }
        done
    });
    f.pos += moved;
    if moved == 0 && n > 0 { -1 } else { moved as i32 }
}

#[no_mangle]
pub extern "C" fn read(fd: i32, buf: *mut u8, n: i32) -> i32 {
    transfer(fd, buf, n, false)
}

#[no_mangle]
pub extern "C" fn write(fd: i32, buf: *const u8, n: i32) -> i32 {
    transfer(fd, buf.cast_mut(), n, true)
}

#[no_mangle]
pub extern "C" fn close(fd: i32) -> i32 {
    match file(fd) {
        Some(f) => {
            f.open = false;
            fd
        }
        None => -1,
    }
}

/// Lay down an empty directory ("bu00:" or "bu10:"): 1 done, 0 not.
#[no_mangle]
pub unsafe extern "C" fn format(dev: *const u8) -> i32 {
    // SAFETY: a C string from the game.
    let Some((slot, _)) = (unsafe { parse(dev) }) else {
        return 0;
    };
    i32::from(with_card(slot, |card| Card::new(Dev(card)).format().is_ok()))
}

/// The card borrowed, as psx-mc's `Card` takes a device by value.
struct Dev<'a>(&'a mut HardwareCard);

impl Block for Dev<'_> {
    fn read_frame(&mut self, frame: u16, out: &mut Frame) -> Result<(), Error> {
        self.0.read_frame(frame, out)
    }
    fn write_frame(&mut self, frame: u16, data: &Frame) -> Result<(), Error> {
        self.0.write_frame(frame, data)
    }
}

/// Free a file's blocks: 1 done, 0 not (no card, no such file).
#[no_mangle]
pub unsafe extern "C" fn delete(path: *const u8) -> i32 {
    // SAFETY: a C string from the game.
    let Some((slot, name)) = (unsafe { parse(path) }) else {
        return 0;
    };
    with_card(slot, |card| {
        let mut dir = [[0; FRAME_SIZE]; BLOCKS];
        if read_into(card, BIOS, &mut dir).is_err() {
            return 0;
        }
        let Some(first) = find_in(&dir, name, BIOS) else {
            return 0;
        };
        let c = chain_in(&dir, first);
                    let (blocks, n) = (c.blocks, c.len);
        for &b in &blocks[..n] {
            let mut e = [0; FRAME_SIZE];
            e[E_STATE] = ST_FREE;
            e[E_LINK..E_LINK + 2].copy_from_slice(&LINK_NONE.to_le_bytes());
            if write_entry(card, b as usize - 1, &mut e).is_err() {
                return 0;
            }
        }
        1
    })
}

// RESTORE: PLATFORM. The directory is read once into a local array (`read_into`) and walked in place
// (`find_in`, `chain_in`): psx-mc's `DirOptions::BIOS` forms, the behaviour this file held as its own
// `name_matches`, `read_dir`, `find` and `chain` until SDK c95fefec (wildcards, a bad checksum accepted,
// a chain that stops at a bad link). tests/psyq-card holds them equal to the old walk on random images.
