//! The boot splash: the studio mark beside the PSX Archive logo, the game's line and the credit
//! line under them.
//!
//! RESTORE: PLATFORM (owner's decisions: 2026-11-13, "Standard splash, new line"; 2026-11-22,
//! "rebuilt with PSoXide in collaboration with PSX Archive", the two "on the same screen"). Not a
//! Psy-Q function. The game calls `PsxSplash` once at boot, after its pad is started and its video
//! mode is set. Declared in `psyq/include/psxsplash.h`.
//!
//! The SDK's `psx-splash` draws one mark and one line, and nothing about it is a parameter, so the
//! layer draws this screen itself with the SDK's pieces and the SDK's own mark
//! (`tools/bonnie_mark.py` cooks it from the pinned SDK's `psx-splash/src/logo.rs` and checks it
//! against that file's hash). The SDK splash's length and skip rule are kept (150 frames, one a
//! VBlank whatever the presentation cap, the last 44 fading out; a fresh press of Cross, Circle or
//! Start after the first 8 frames ends it), and its lines' look (the 8x8 `BASIC` font with
//! a black outline, the same colours and sheen). On that the screen is animated (owner, 2026-11-22:
//! "make it fancier"): the mark slides in from the left and the logo from the right, eased, a
//! glint crosses both, and the two lines rise in under them, each sheen sweeping once.

use psx_font::{fonts::BASIC, FontAtlas};
use psx_gpu::display::{DisplayConfig, DoubleBuffer, Resolution, VideoMode};
use psx_gpu::material::{BlendMode, TextureMaterial};
use psx_gpu::prim::{QuadTexturedGouraud, QuadTexturedMaterial};
use psx_gpu::Gpu;
use psx_io::periph::GpuDma;
use psx_pad::{button, ButtonState};
use psx_vram::{upload_16bpp, Clut, TextureDepth, TexturePage, VramRect};

#[repr(C, align(4))]
struct Packed<T: ?Sized>(T);

/// The Bonnie Studios mark: 16 palette words, then 128 by 128 at 4 bits a pixel (32 words a
/// row). Cooked by `tools/bonnie_mark.py` at build time (`psyq/build.sh`).
#[link_section = ".boot_only.splash_mark"]
static MARK_DATA: Packed<[u8; 32 + 8192]> = Packed(*include_bytes!("../../build/bonniemark.bin"));

/// The PSX Archive logo, packed from `psyq/art/psxarchive-logo.png` by `tools/psxarchive_logo.py`
/// (`psyq/build.sh`): width and height, a 256-colour palette, then one byte a pixel.
const LOGO_LEN: usize = include_bytes!("../../build/psxarchive.bin").len();
#[link_section = ".boot_only.splash_logo"]
static LOGO: Packed<[u8; LOGO_LEN]> = Packed(*include_bytes!("../../build/psxarchive.bin"));

#[link_section = ".boot_only.splash_text"]
static CREDIT: [u8; 33] = *b"in collaboration with PSX Archive";

const MARK_TPAGE: TexturePage = TexturePage::new(896, 0, TextureDepth::Bit4);
const MARK_CLUT: Clut = Clut::new(768, 256);
const LOGO_TPAGE: TexturePage = TexturePage::new(640, 0, TextureDepth::Bit8);
const LOGO_CLUT: Clut = Clut::new(768, 258);
const FONT_TPAGE: TexturePage = TexturePage::new(320, 0, TextureDepth::Bit4);
const FONT_CLUT: Clut = Clut::new(320, 256);

const MARK_GLINT_CLUT: Clut = Clut::new(784, 256);
const LOGO_GLINT_CLUT: Clut = Clut::new(768, 259);

/// The SDK splash's length and fade out; the entrances inside it: each element eases in over
/// `ENTER` frames from its start, sliding `SLIDE` pixels; the glint crosses in `GLINT` frames.
const TOTAL: i32 = 150;
const FADE_OUT: i32 = 44;
const ENTER: i32 = 28;
const SLIDE: i32 = 40;
const MARK_AT: i32 = 0;
const LOGO_AT: i32 = 12;
const LINE_AT: i32 = 30;
const CREDIT_AT: i32 = 40;
const GLINT_AT: i32 = 46;
const GLINT: i32 = 40;
/// The glint's half width in pixels, its brightest tint, and the strips it is drawn in.
const GLINT_BAND: i32 = 22;
const GLINT_PEAK: i32 = 0x70;
const GLINT_STRIP: i16 = 8;
const SKIP_GRACE: i32 = 8;
const SKIP: u16 = button::CROSS | button::CIRCLE | button::START;
/// The mark's size on screen, the gap between the two pictures, and between the pictures and
/// the lines; half the sheen's width in letters.
const MARK_SIZE: i16 = 96;
const GAP: i16 = 24;
const LINES_GAP: i16 = 16;
const SHEEN: i32 = 18;

/// Play the splash with the NUL-terminated `line` (the game's "Rebuilt with PSoXide") and return
/// when it has run or a press of Cross, Circle or Start has ended it. The pad is the one the game
/// gave `InitPAD` (port 1), as the VBlank poll fills it; a game that shows the splash before
/// `InitPAD` gets a buffer of the layer's own, polled the same way.
///
/// Takes over the GPU (reset, 320 by 240) and leaves it showing a black frame; the caller sets its
/// own display up afterwards, as it does after `FmvPlay`. VRAM is left as the splash used it (the
/// mark at x 896, y 0 and its palette at x 768, y 256; the logo at x 640, y 0, 8 bits a pixel, and
/// its palette at x 768, y 258; the 8x8 font at x 320, y 0 and its palette at x 320, y 256), which
/// a game's own uploads overwrite.
#[no_mangle]
pub unsafe extern "C" fn PsxSplash(line: *const u8) {
    let mut len = 0;
    // SAFETY: the game passes a NUL-terminated string.
    while unsafe { *line.add(len) } != 0 {
        len += 1;
    }
    // SAFETY: as above, `len` bytes of it.
    let Ok(line) = core::str::from_utf8(unsafe { core::slice::from_raw_parts(line, len) }) else {
        return;
    };
    // The splash waits on psx-rt's VBlank counter.
    crate::kernel::ensure_interrupts();
    crate::gpu::wait_idle();
    let mode = if crate::gpu::video_mode() == 1 { VideoMode::Pal } else { VideoMode::Ntsc };
    // SAFETY: this layer is the only driver of GP0 and DMA channel 2 (gpu.rs).
    let mut gpu = Gpu::new(unsafe { GpuDma::steal() }, DisplayConfig::new(mode, Resolution::R320X240));
    let mut buffers = DoubleBuffer::new(Resolution::R320X240);
    buffers.apply_draw_target(&mut gpu);
    let early = crate::kernel::early_pad_begin();
    show(&mut gpu, &mut buffers, line);
    if early {
        crate::kernel::early_pad_end();
    }
}

/// Words of a packed picture. Every part of `MARK_DATA` and `LOGO` is a whole number of words and
/// both are 4-aligned.
fn words(bytes: &[u8]) -> &[u16] {
    // SAFETY: as above; the slice covers the bytes exactly.
    unsafe { core::slice::from_raw_parts(bytes.as_ptr().cast::<u16>(), bytes.len() / 2) }
}

/// A picture on screen: where it is drawn, its texels' size, and its two materials (opaque, and
/// the additive one the glint is drawn with).
#[derive(Clone, Copy)]
struct Picture {
    x: i16,
    y: i16,
    w: i16,
    h: i16,
    tw: u8,
    th: u8,
    base: TextureMaterial,
    glint: TextureMaterial,
}

/// Ease-out offset for an element that enters over `ENTER` frames starting at `at`: `distance`
/// at the start, 0 at the end; and its fade level (0 to 0x80).
fn enter(frame: i32, at: i32, distance: i32) -> (i16, i32) {
    let t = (frame - at).clamp(0, ENTER);
    let rest = ENTER - t;
    ((distance * rest * rest / (ENTER * ENTER)) as i16, t * 0x80 / ENTER)
}

#[link_section = ".boot_only.text.splash_show"]
#[inline(never)]
fn show(gpu: &mut Gpu, buffers: &mut DoubleBuffer, line: &str) {
    let mark = words(&MARK_DATA.0);
    upload_16bpp(VramRect::new(MARK_CLUT.x(), MARK_CLUT.y(), 16, 1), &mark[..16]);
    upload_16bpp(VramRect::new(MARK_TPAGE.x(), MARK_TPAGE.y(), 32, 128), &mark[16..]);
    let logo = words(&LOGO.0);
    let (lw, lh) = (logo[0], logo[1]);
    upload_16bpp(VramRect::new(LOGO_CLUT.x(), LOGO_CLUT.y(), 256, 1), &logo[2..258]);
    upload_16bpp(VramRect::new(LOGO_TPAGE.x(), LOGO_TPAGE.y(), lw / 2, lh), &logo[258..]);
    // The glint's palettes: the same colours with the semi-transparency bit, so every texel but the
    // transparent ones adds (a textured primitive blends only the texels whose bit 15 is set).
    let mut stp = [0u16; 256];
    for (to, from) in stp.iter_mut().zip(&logo[2..258]) {
        *to = if *from == 0 { 0 } else { *from | 0x8000 };
    }
    upload_16bpp(VramRect::new(LOGO_GLINT_CLUT.x(), LOGO_GLINT_CLUT.y(), 256, 1), &stp);
    for (to, from) in stp.iter_mut().zip(&mark[..16]) {
        *to = if *from == 0 { 0 } else { *from | 0x8000 };
    }
    upload_16bpp(VramRect::new(MARK_GLINT_CLUT.x(), MARK_GLINT_CLUT.y(), 16, 1), &stp[..16]);
    let font = FontAtlas::upload(&BASIC, FONT_TPAGE, FONT_CLUT);
    // SAFETY: ASCII.
    let credit = unsafe { core::str::from_utf8_unchecked(&CREDIT) };

    // The two pictures side by side, centred on the logo's height, the two lines under them; the
    // whole block centred on the screen.
    let (lw, lh) = (lw as i16, lh as i16);
    let pictures = lh.max(MARK_SIZE);
    let top = (240 - (pictures + LINES_GAP + 8 + 6 + 8)) / 2;
    let x0 = (320 - (MARK_SIZE + GAP + lw)) / 2;
    let mark_pic = Picture {
        x: x0,
        y: top + (pictures - MARK_SIZE) / 2,
        w: MARK_SIZE,
        h: MARK_SIZE,
        tw: 128,
        th: 128,
        base: TextureMaterial::opaque(MARK_CLUT.uv_word(), MARK_TPAGE.uv_word(0), (0, 0, 0)),
        glint: TextureMaterial::blended(MARK_GLINT_CLUT.uv_word(), MARK_TPAGE.uv_word(0), (0, 0, 0), BlendMode::Add),
    };
    let logo_pic = Picture {
        x: x0 + MARK_SIZE + GAP,
        y: top + (pictures - lh) / 2,
        w: lw,
        h: lh,
        tw: lw as u8,
        th: lh as u8,
        base: TextureMaterial::opaque(LOGO_CLUT.uv_word(), LOGO_TPAGE.uv_word(0), (0, 0, 0)),
        glint: TextureMaterial::blended(LOGO_GLINT_CLUT.uv_word(), LOGO_TPAGE.uv_word(0), (0, 0, 0), BlendMode::Add),
    };
    let line_y = top + pictures + LINES_GAP;
    let credit_y = line_y + 8 + 6;
    let centred = |text: &str| 160 - (font.text_width(text) / 2) as i16;
    let (line_x, credit_x) = (centred(line), centred(credit));
    // The glint's path: a diagonal band from left of the mark to right of the logo.
    let (glint_from, glint_to) = (i32::from(x0) - GLINT_BAND - i32::from(pictures) / 2, 320 - i32::from(x0) + GLINT_BAND);

    let cap = psx_rt::present::cap();
    psx_rt::present::set_cap(psx_rt::present::PresentCap::UNCAPPED);
    let skips = |b: ButtonState| b.is_held(SKIP);
    let mut previous = crate::kernel::pad1_buttons();
    let mut frame = 0;
    while frame < TOTAL {
        let now = crate::kernel::pad1_buttons();
        if frame > SKIP_GRACE && skips(now) && !skips(previous) {
            break;
        }
        previous = now;
        // The whole screen fades out together at the end.
        let out = if frame < TOTAL - FADE_OUT { 0x80 } else { (TOTAL - frame) * 0x80 / FADE_OUT };

        buffers.clear(gpu, (0, 0, 0));
        // The mark slides in from the left, the logo from the right a little after it.
        let (dx, level) = enter(frame, MARK_AT, SLIDE);
        draw_picture(gpu, &mark_pic, -dx, level.min(out));
        let (dx, level) = enter(frame, LOGO_AT, SLIDE);
        draw_picture(gpu, &logo_pic, dx, level.min(out));
        // Then the glint sweeps across both, once.
        let t = frame - GLINT_AT;
        if (0..GLINT).contains(&t) {
            let at = glint_from + (glint_to - glint_from) * t / GLINT;
            draw_glint(gpu, &mark_pic, at);
            draw_glint(gpu, &logo_pic, at);
        }
        // The lines rise into place under them, one after the other, with the splash's sheen.
        let head = (frame - LINE_AT) / 2;
        let (dy, level) = enter(frame, LINE_AT, 8);
        draw_line(&font, line_x, line_y + dy, line, head, level.min(out));
        let (dy, level) = enter(frame, CREDIT_AT, 8);
        draw_line(&font, credit_x, credit_y + dy, credit, head - 4, level.min(out));
        gpu.wait_idle();
        if !psx_rt::interrupts::try_wait_vblank(4_000_000) {
            break;
        }
        buffers.swap(gpu);
        frame += 1;
    }
    psx_rt::present::set_cap(cap);
}

/// A picture, `dx` pixels off its place, at fade `level` (0 to 0x80).
#[link_section = ".boot_only.text.splash_picture"]
#[inline(never)]
fn draw_picture(gpu: &mut Gpu, p: &Picture, dx: i16, level: i32) {
    if level <= 0 {
        return;
    }
    let grey = level as u8;
    let (x, y) = (p.x + dx, p.y);
    gpu.draw(&QuadTexturedMaterial::with_material(
        [(x, y), (x + p.w, y), (x, y + p.h), (x + p.w, y + p.h)],
        [(0, 0), (p.tw, 0), (0, p.th), (p.tw, p.th)],
        p.base.with_tint((grey, grey, grey)),
    ));
}

/// The glint over a picture: the picture again, added over itself in vertical strips whose
/// corners are lit by their distance from a diagonal line through x `at` (at the picture's top,
/// leaning right as it goes down), so a soft bright band crosses it.
#[link_section = ".boot_only.text.splash_glint"]
#[inline(never)]
fn draw_glint(gpu: &mut Gpu, p: &Picture, at: i32) {
    let light = |x: i16, y: i16| {
        let d = (i32::from(x) - (at + i32::from(y - p.y) / 2)).abs();
        let v = (GLINT_PEAK * (GLINT_BAND - d) / GLINT_BAND).max(0) as u8;
        (v, v, v)
    };
    let (y0, y1) = (p.y, p.y + p.h);
    let strips = (p.w + GLINT_STRIP - 1) / GLINT_STRIP;
    for i in 0..strips {
        let (sx0, sx1) = (i * GLINT_STRIP, ((i + 1) * GLINT_STRIP).min(p.w));
        let (x0, x1) = (p.x + sx0, p.x + sx1);
        let lit = [light(x0, y0), light(x1, y0), light(x0, y1), light(x1, y1)];
        if lit.iter().all(|c| c.0 == 0) {
            continue;
        }
        let u = |sx: i16| (i32::from(sx) * i32::from(p.tw) / i32::from(p.w)) as u8;
        gpu.draw(&QuadTexturedGouraud::with_material(
            [(x0, y0), (x1, y0), (x0, y1), (x1, y1)],
            [(u(sx0), 0), (u(sx1), 0), (u(sx0), p.th), (u(sx1), p.th)],
            lit,
            p.glint,
        ));
    }
}

/// A line as the SDK's splash draws its own: a black outline, then each letter as a gradient
/// whose brightness rises toward the sheen's head, all scaled by the fade `level` (0 to 0x80).
#[link_section = ".boot_only.text.splash_line"]
#[inline(never)]
fn draw_line(font: &FontAtlas, mut x: i16, y: i16, line: &str, head: i32, level: i32) {
    if level <= 0 {
        return;
    }
    let mix = |colour: (u8, u8, u8), near: i32| -> (u8, u8, u8) {
        let channel = |value: u8| {
            let base = i32::from(value) * level / 0x80;
            (base + (level - base) * near / SHEEN) as u8
        };
        (channel(colour.0), channel(colour.1), channel(colour.2))
    };
    for (index, ch) in line.char_indices() {
        let mut one = [0u8; 4];
        let glyph: &str = ch.encode_utf8(&mut one);
        for (dx, dy) in [(-1, -1), (1, -1), (-1, 1), (1, 1)] {
            font.draw_text(x + dx, y + dy, glyph, (0, 0, 0));
        }
        let near = (SHEEN - (index as i32 - head).abs() * 6).max(0);
        font.draw_text_gradient(
            x,
            y,
            glyph,
            mix((0x68, 0x80, 0x80), near),
            mix((0x38, 0x58, 0x80), near),
        );
        // `BASIC` is fixed width, so every glyph advances the same.
        x += i16::from(font.font().advance_x);
    }
}
