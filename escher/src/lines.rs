//! E_Draw2DLine's queue: the game queues lines (the head string, the
//! tracking lines, the camera cross) before a render call and resets the
//! queue after it, so they are drawn with that call's band, at the near end
//! of their sort value's rank.

use crate::pool::{self, Line2D};
use crate::render::screen;

static mut COUNT: usize = 0;

pub fn queued() -> &'static [Line2D] {
    // SAFETY: foreground-only static; the queue lives in E_Init's memory.
    unsafe {
        let l = pool::lines();
        &l[..COUNT.min(l.len())]
    }
}

pub fn reset() {
    // SAFETY: foreground-only static.
    unsafe { COUNT = 0 };
}

/// x in eighths of a PC pixel, y in PC pixels (STV3D.C Draw_Line), colour a
/// ramp number: the line takes the base colour of that ball-colour ramp,
/// POOLPAL + 2560 + 16 * colour (9: the shadow ramp, the felt-dark line under
/// the tracking lines and the head string). That reading is this renderer's.
pub fn add(x0: i32, y0: i32, x1: i32, y1: i32, colour: i32, sort: i32) {
    let s = screen();
    let px = |x: i32| (x * 320 / s.w.max(1)) >> 3;
    let py = |y: i32| y * 256 / s.h.max(1);
    let pal = crate::balls::ramp_base(colour);
    // SAFETY: foreground-only static.
    unsafe {
        let l = pool::lines();
        if COUNT < l.len() {
            l[COUNT] = Line2D {
                x0: px(x0).clamp(-1024, 1023) as i16,
                y0: py(y0).clamp(-1024, 1023) as i16,
                x1: px(x1).clamp(-1024, 1023) as i16,
                y1: py(y1).clamp(-1024, 1023) as i16,
                colour: pal,
                sort,
            };
            COUNT += 1;
        }
    }
}
