//! The debug font: text streams printed into and drawn on top of a frame.
//!
//! `FntLoad(tx, ty)` puts psx-font's 8x8 public-domain VGA font in VRAM at
//! (tx, ty) as a 4-bit texture with its two-colour CLUT 128 lines below.
//! `FntOpen` opens a stream (a window on the screen and a character
//! budget), `FntPrint` appends formatted text to the stream `SetDumpFnt`
//! chose, and `FntFlush` draws a stream's text and empties it. The glyphs are
//! PSoXide's, not Sony's, so the overlay matches the 1996 build in content,
//! position and case (capitals only, as that build shows it), not in pixels.

use psx_font::FontAtlas;
use psx_vram::{Clut, TextureDepth, TexturePage};

const STREAMS: usize = 4;
const TEXT_BYTES: usize = 1024;

struct Stream {
    open: bool,
    x: i16,
    y: i16,
    w: i16,
    h: i16,
    limit: usize,
    len: usize,
    text: [u8; TEXT_BYTES],
}

const CLOSED: Stream = Stream { open: false, x: 0, y: 0, w: 0, h: 0, limit: 0, len: 0, text: [0; TEXT_BYTES] };
static mut STREAMS_: [Stream; STREAMS] = [CLOSED; STREAMS];
static mut DUMP: i32 = -1;
static mut ATLAS: Option<FontAtlas> = None;

#[allow(clippy::mut_from_ref)]
fn streams() -> &'static mut [Stream; STREAMS] {
    // SAFETY: only the foreground touches the streams, one call at a time.
    unsafe { &mut *(&raw mut STREAMS_) }
}

#[no_mangle]
pub extern "C" fn FntLoad(tx: i32, ty: i32) {
    crate::gpu::wait_idle();
    let tpage = TexturePage::new(tx as u16 & 0x3C0, ty as u16 & 0x100, TextureDepth::Bit4);
    let clut = Clut::new(tx as u16 & 0x3F0, (ty as u16 & 0x100) + 128);
    let atlas = FontAtlas::upload(&psx_font::fonts::BASIC, tpage, clut);
    // SAFETY: foreground-only static.
    unsafe { *(&raw mut ATLAS) = Some(atlas) };
    for s in streams().iter_mut() {
        s.open = false;
    }
}

#[no_mangle]
pub extern "C" fn FntOpen(x: i32, y: i32, w: i32, h: i32, _isbg: i32, n: i32) -> i32 {
    for (i, s) in streams().iter_mut().enumerate() {
        if !s.open {
            s.open = true;
            s.x = x as i16;
            s.y = y as i16;
            s.w = w as i16;
            s.h = h as i16;
            s.limit = (n.max(0) as usize).min(TEXT_BYTES);
            s.len = 0;
            return i as i32;
        }
    }
    -1
}

#[no_mangle]
pub extern "C" fn SetDumpFnt(id: i32) {
    // SAFETY: foreground-only static.
    unsafe { core::ptr::write_volatile(&raw mut DUMP, id) };
}

fn stream(id: i32) -> Option<&'static mut Stream> {
    let id = if id < 0 {
        // SAFETY: foreground-only static.
        unsafe { core::ptr::read_volatile(&raw const DUMP) }
    } else {
        id
    };
    streams().get_mut(usize::try_from(id).ok()?).filter(|s| s.open)
}

#[no_mangle]
pub unsafe extern "C" fn FntPrint(fmt: *const u8, mut args: ...) -> i32 {
    let Some(s) = stream(-1) else { return 0 };
    let mut n = 0;
    // SAFETY: the game's format string and its arguments.
    unsafe {
        crate::libc::format(fmt, &mut args, &mut |c| {
            if s.len < s.limit {
                // NMS's 1996 build shows its lower-case overlay text in capitals
                // (the dev kit font had no lower case): do the same.
                s.text[s.len] = c.to_ascii_uppercase();
                s.len += 1;
            }
            n += 1;
        });
    }
    n
}

/// Draw stream `id` (-1: the one SetDumpFnt chose) and empty it.
#[no_mangle]
pub extern "C" fn FntFlush(id: i32) -> *mut u32 {
    let Some(s) = stream(id) else { return core::ptr::null_mut() };
    // SAFETY: foreground-only static.
    let Some(atlas) = (unsafe { (*(&raw const ATLAS)).as_ref() }) else {
        s.len = 0;
        return core::ptr::null_mut();
    };
    crate::gpu::wait_idle();
    let line = atlas.line_height() as i16;
    let (mut x, mut y) = (s.x, s.y);
    let mut start = 0;
    let text = &s.text[..s.len];
    for i in 0..=text.len() {
        let end = i == text.len() || text[i] == b'\n';
        if end {
            if let Ok(run) = core::str::from_utf8(&text[start..i]) {
                if y + line <= s.y + s.h {
                    atlas.draw_text(x, y, run, (0x80, 0x80, 0x80));
                }
            }
            start = i + 1;
            x = s.x;
            y += line;
        }
    }
    s.len = 0;
    core::ptr::null_mut()
}
