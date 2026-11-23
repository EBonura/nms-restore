//! The balls: STV3D.C's DrawAllBalls draws each one with E_DrawBall as a
//! shaded disc (and its shadow as a squashed flat one), then renders the
//! ball's spot, stripe and number meshes over it. Here the disc is a
//! textured quad: one 64x64 4-bit disc whose texels are shade steps, and a
//! colour table (CLUT) per ball colour built from the game's pool palette
//! table (POOLPAL.TAB block 10, as GetBallPtrs reads it: 16 entries per
//! colour, 8..15 the shading from the colour to white).
//!
//! VRAM: the disc at (640, 0), 16 x 64 halfwords; the CLUTs at (640, 64..83),
//! clear of the game's (frame buffers 0..319 x 0..511, the 2D ball sprite at
//! 320,0 and its CLUT row at 320,511, the debug font at 960,256).

use psx_math::int32::isqrt_u32 as isqrt;
use crate::nms::{LoadImage, Rect};
use crate::pool;
use crate::render::{emit_sprite, rgb_of, screen, take_slot};

const TEX_X: i16 = 640;
const TEX_Y: i16 = 0;
const CLUT_Y: i16 = 64;
const SIZE: i32 = 64;
/// tpage: x 640/64 = 10, y 0, 4-bit
const TPAGE: u32 = 10;

static mut DIRTY: bool = true;

pub fn mark_dirty() {
    // SAFETY: foreground-only static.
    unsafe { DIRTY = true };
}

/// The palette index at the base of ball-colour ramp `colour` (0..9, the
/// game's col/16; 9 is SHADOW_COLOUR, 16*9).
pub fn ramp_base(colour: i32) -> i32 {
    ramp(colour.clamp(0, 15) * 16)
}

fn ramp(i: i32) -> i32 {
    let t = pool::pooltab();
    if t.is_null() {
        return 0;
    }
    // SAFETY: the 2816-byte POOLPAL buffer; block 10 is 2560..2815.
    unsafe { *t.add(2560 + (i & 255) as usize) as i32 }
}

fn bgr15(index: i32) -> u16 {
    let c = rgb_of(index);
    let (r, g, b) = ((c & 0xFF) >> 3, ((c >> 8) & 0xFF) >> 3, ((c >> 16) & 0xFF) >> 3);
    // bit 15 set: opaque even when black (0x0000 is the transparent texel)
    0x8000 | (r | (g << 5) | (b << 10)) as u16
}

/// Build and load the disc and the CLUTs, once the room's palette and pool
/// table are in (the first frame after init_3d). `buf` is free scratch of
/// at least 2 KB (the page's primitive buffer, before any primitive).
pub fn upload_if_needed(buf: *mut u8) {
    // SAFETY: foreground-only static; `buf` per the contract above.
    unsafe {
        if !DIRTY || buf.is_null() || pool::pooltab().is_null() {
            return;
        }
        DIRTY = false;
        // the disc: texel 1 + shade (0..7, toward the highlight up and left),
        // 0 outside
        let tex = core::slice::from_raw_parts_mut(buf, (SIZE * SIZE / 2) as usize);
        for ty in 0..SIZE {
            for tx in 0..SIZE {
                let (dx, dy) = (2 * tx + 1 - SIZE, 2 * ty + 1 - SIZE);
                let mut v = 0u8;
                if dx * dx + dy * dy <= SIZE * SIZE {
                    let (hx, hy) = (dx + 22, dy + 26);
                    let d = isqrt((hx * hx + hy * hy) as u32) as i32;
                    v = 1 + (7 - (d * 15) / 128).clamp(0, 7) as u8;
                }
                let i = (ty * SIZE + tx) as usize;
                if tx & 1 == 0 {
                    tex[i / 2] = v;
                } else {
                    tex[i / 2] |= v << 4;
                }
            }
        }
        let r = Rect { x: TEX_X, y: TEX_Y, w: (SIZE / 4) as i16, h: SIZE as i16 };
        LoadImage(&r, buf);
        // CLUT k (0..9): the shading ramp of colour 16k; CLUT 10+k: flat, the
        // ramp's base colour (shadows)
        let clut = core::slice::from_raw_parts_mut(buf.cast::<u16>(), 16 * 20);
        for k in 0..10 {
            clut[16 * k] = 0;
            clut[16 * (10 + k)] = 0;
            for j in 1..16 {
                let step = (j as i32 - 1).min(7);
                clut[16 * k + j] = bgr15(ramp(16 * k as i32 + 8 + step));
                clut[16 * (10 + k) + j] = bgr15(ramp(16 * k as i32));
            }
        }
        let r = Rect { x: TEX_X, y: CLUT_Y, w: 16, h: 20 };
        LoadImage(&r, buf);
    }
}

/// E_DrawBall: x, y and radius in eighths of a PC pixel, `shape` the
/// vertical squash (16.16), `hilite` shaded (1) or flat (0).
pub fn draw(x8: i32, y8: i32, r8: i32, colour: i32, shape: i32, hilite: i32) {
    let Some(slot) = take_slot() else { return };
    let s = screen();
    let w = s.w.max(1);
    let h = s.h.max(1);
    // PC eighth-pixels to PS1 pixels (x * 320 / w / 8, y * 256 / h / 8)
    let cx = x8 * 40 / w;
    let cy = y8 * 32 / h;
    let r = r8 * 40 / w;
    if r < 1 {
        return;
    }
    // round on the 4:3 screen: squash, the PC pixel aspect, PC to PS1 lines
    // (all 16.16)
    let f1 = ((shape.clamp(0, 65536) >> 4) * (s.aspect >> 4)) >> 8;
    let f2 = (((256 << 16) / h) * w) / 320;
    let f = ((f1 >> 4) * (f2 >> 4)) >> 8;
    let ry = ((r * f) >> 16).max(1);
    let k = (colour >> 4).clamp(0, 9) as u32;
    let k = if hilite != 0 { k } else { 10 + k };
    let clut = ((CLUT_Y as u32 + k) << 6) | (TEX_X as u32 >> 4);
    emit_sprite(slot, cx - r, cy - ry, cx + r, cy + ry, clut, TPAGE, (SIZE - 1) as u32);
}
