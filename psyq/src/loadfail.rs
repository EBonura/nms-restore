//! RESTORE: PLATFORM, not a Psy-Q call. The screen a game shows when a file cannot be loaded.
//!
//! Both games used to carry on with whatever a failed load left (a null pointer, a half-read
//! buffer), which on a console showed as a freeze with nothing on the screen to say why. Their
//! loaders now call `PsyqLoadFailed` instead: it takes the screen over (320 x 240, not
//! interlaced, at VRAM 0,0, in the video standard the game set), writes in the SDK's 8 x 8 font
//! (psx-font's bitmap, copied to VRAM glyph by glyph, so no code that 360X keeps in boot-only
//! memory is needed) the file, the loader's step and, when the drive failed, the drive's side of
//! it (cd.rs: which part of the read, the tries, the reader's diagnostic bytes, the sector), and
//! stops there. One look at the console says what went wrong. It draws once and then spins with
//! no interrupt, DMA or drive use, so nothing it relies on can fail after it.

use crate::cd::{last_failure, STEP_PREPARE, STEP_SECTOR, STEP_SEEK};
use crate::gpu::{DispEnv, DrawEnv, Rect};

/// The screen's text: at most 36 characters a line (8-pixel glyphs from x 16).
struct Text {
    bytes: [u8; 320],
    len: usize,
}

impl Text {
    #[inline(never)]
    fn push(&mut self, b: &[u8]) {
        for &c in b {
            if let Some(slot) = self.bytes.get_mut(self.len) {
                *slot = c.to_ascii_uppercase();
                self.len += 1;
            }
        }
    }

    #[inline(never)]
    fn hex(&mut self, v: u32, digits: u32) {
        for i in (0..digits).rev() {
            let d = ((v >> (i * 4)) & 15) as u8;
            self.push(&[if d < 10 { b'0' + d } else { b'A' + d - 10 }]);
        }
    }

    #[inline(never)]
    fn dec(&mut self, v: u32) {
        let mut div = 1_000_000_000;
        let mut started = false;
        while div > 0 {
            let d = (v / div % 10) as u8;
            if d != 0 || started || div == 1 {
                self.push(&[b'0' + d]);
                started = true;
            }
            div /= 10;
        }
    }

    /// A C string, at most `max` bytes of it.
    #[inline(never)]
    unsafe fn c_str(&mut self, s: *const u8, max: usize) {
        if s.is_null() {
            self.push(b"?");
            return;
        }
        for i in 0..max {
            // SAFETY: the caller's NUL-terminated string, read up to its NUL or `max`.
            let c = unsafe { *s.add(i) };
            if c == 0 {
                break;
            }
            self.push(&[c]);
        }
    }
}

/// The first 16 bytes of the name of the file the screen is up for (zero until then): what a
/// test journey reads (tests/qdup-cd-fail.toml, x360-cd-fail.toml).
#[no_mangle]
static mut PSYQ_LOAD_FAILED: [u8; 16] = [0; 16];

/// Show that `file` could not be loaded at the loader's step `step` (both C strings), and stop.
///
/// The lines: LOAD FAILED; FILE and the name; STEP and the loader's step; then, when the last
/// read failed in the drive, DRIVE and the part of the read (SETMODE, SEEK, SECTOR) with the
/// tries, ERROR and the reader's diagnostic bytes (cause FF a wait spun out, 05 the drive answered
/// with an error, FE its parameter FIFO stayed full, FD a Pause not proved; then the status byte,
/// the command and the IRQ flag or error code), and LBA, the sector in the program's image and on
/// the disc (they differ on a demo disc).
#[cold]
#[no_mangle]
pub unsafe extern "C" fn PsyqLoadFailed(file: *const u8, step: *const u8) -> ! {
    let mut t = Text { bytes: [0; 320], len: 0 };
    t.push(b"LOAD FAILED\n\nFILE  ");
    // SAFETY: the game's strings.
    unsafe { t.c_str(file, 30) };
    t.push(b"\nSTEP  ");
    // SAFETY: as above.
    unsafe { t.c_str(step, 30) };
    t.push(b"\n");
    match last_failure() {
        Some(f) => {
            t.push(b"DRIVE ");
            t.push(match f.step {
                STEP_PREPARE => b"SETMODE".as_slice(),
                STEP_SEEK => b"SEEK".as_slice(),
                STEP_SECTOR => b"SECTOR".as_slice(),
                _ => b"?".as_slice(),
            });
            t.push(b"  TRIES ");
            t.dec(u32::from(f.tries));
            t.push(b"\nERROR ");
            for i in (0..4).rev() {
                t.hex(f.diag >> (i * 8), 2);
                t.push(b" ");
            }
            t.push(b"\nLBA   ");
            t.dec(f.lba);
            // the sector on the disc: on a demo disc the program's image starts further in
            t.push(b"  DISC ");
            t.dec(psx_io::disc_base::shift_lba(f.lba));
            t.push(b"\n");
        }
        None => t.push(b"DRIVE NO READ FAILED\n"),
    }

    crate::gpu::wait_idle();
    crate::gpu::SetDispMask(0);
    // SAFETY: zeroed environments are valid (plain integers); the Def calls fill them.
    let mut disp: DispEnv = unsafe { core::mem::zeroed() };
    let mut draw: DrawEnv = unsafe { core::mem::zeroed() };
    // SAFETY: the layer's own calls on local records. The drawing environment's background fill
    // clears the screen to the blue.
    unsafe {
        crate::gpu::SetDefDispEnv(&mut disp, 0, 0, 320, 240);
        crate::gpu::SetDefDrawEnv(&mut draw, 0, 0, 320, 240);
        draw.dfe = 1;
        draw.isbg = 1;
        draw.b0 = BACK;
        crate::gpu::PutDispEnv(&mut disp);
        crate::gpu::PutDrawEnv(&mut draw);
    }
    draw_text(t.bytes.get(..t.len).unwrap_or(&[]));
    crate::gpu::wait_idle();
    crate::gpu::SetDispMask(1);
    // SAFETY: the one writer, after which nothing else runs.
    unsafe {
        let name = &mut *(&raw mut PSYQ_LOAD_FAILED);
        for (i, slot) in name.iter_mut().enumerate().take(15) {
            let c = if file.is_null() { 0 } else { *file.add(i) };
            *slot = c;
            if c == 0 {
                break;
            }
        }
    }
    loop {
        core::hint::spin_loop();
    }
}

/// The screen's blue (8-bit), and the same as a 15-bit pixel.
const BACK: u8 = 64;
const BACK_PIXEL: u16 = (BACK as u16 >> 3) << 10;

/// Write `text` from (16, 24), one 8 x 8 glyph a character, white on the blue, a new line every
/// 10 rows at `\n`: each glyph's pixels copied to VRAM (`LoadImage`).
#[inline(never)]
fn draw_text(text: &[u8]) {
    let font = &psx_font::fonts::basic::BASIC_BITMAP;
    let (mut x, mut y) = (16i16, 24i16);
    let mut cell = [0u16; 64];
    for &c in text {
        if c == b'\n' {
            x = 16;
            y += 10;
            continue;
        }
        let at = usize::from(c & 0x7f) * 8;
        let Some(glyph) = font.get(at..at + 8) else { continue };
        for (row, bits) in cell.chunks_exact_mut(8).zip(glyph) {
            for (col, pixel) in row.iter_mut().enumerate() {
                *pixel = if (bits >> col) & 1 != 0 { 0x7fff } else { BACK_PIXEL };
            }
        }
        let rect = Rect { x, y, w: 8, h: 8 };
        // SAFETY: a local 8 x 8 picture of 16-bit pixels.
        unsafe { crate::gpu::LoadImage(&rect, cell.as_ptr().cast()) };
        x += 8;
    }
}
