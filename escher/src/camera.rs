//! The camera: E_PositionCamera places it `distance` behind a target along
//! its rotation, E_EaseCamera/E_UpdateCamera move it there over frames.

use crate::fixed::{apply, rotation};
use crate::nms::Vertex;

pub struct Camera {
    /// where it is now (what E_GetCameraPos/Rot report)
    pub pos: Vertex,
    pub rot: Vertex,
    /// where E_PositionCamera last asked it to be
    want_pos: Vertex,
    want_rot: Vertex,
    /// E_EaseCamera's value: 0 moves at once, n eases (1/n of the way a frame)
    pub ease: i32,
    pub easing: bool,
}

pub static mut CAMERA: Camera = Camera {
    pos: Vertex { x: 0, y: 0, z: -256000 },
    rot: Vertex { x: 0, y: 0, z: 0 },
    want_pos: Vertex { x: 0, y: 0, z: -256000 },
    want_rot: Vertex { x: 0, y: 0, z: 0 },
    ease: 0,
    easing: false,
};

pub fn camera() -> &'static mut Camera {
    // SAFETY: only the game's one thread calls the renderer.
    unsafe { &mut *(&raw mut CAMERA) }
}

/// Wrapped difference of two angles, -2048..2047.
fn angle_diff(to: i32, from: i32) -> i32 {
    ((to - from + 2048) & 4095) - 2048
}

impl Camera {
    pub fn position(&mut self, target: &Vertex, rot: &Vertex, distance: i32) {
        let m = rotation(rot.x, rot.y, rot.z);
        let back = apply(&m, &Vertex { x: 0, y: 0, z: -distance });
        self.want_pos = Vertex { x: target.x + back.x, y: target.y + back.y, z: target.z + back.z };
        self.want_rot = *rot;
        if self.ease <= 0 {
            self.pos = self.want_pos;
            self.rot = self.want_rot;
            self.easing = false;
        } else {
            self.easing = true;
        }
    }

    /// One frame of easing: a 1/n step toward the goal, exponential as a
    /// camera swoop; it snaps the last few units (the easing curve is this
    /// renderer's choice, Escher's is lost).
    pub fn update(&mut self) {
        if !self.easing {
            return;
        }
        let n = self.ease.max(2);
        let (p, wp) = (self.pos, self.want_pos);
        let (r, wr) = (self.rot, self.want_rot);
        let mut done = true;
        let mut step = |cur: i32, want: i32, d: i32, snap: i32| -> i32 {
            if d.abs() <= snap || d / n == 0 {
                want
            } else {
                done = false;
                cur + d / n
            }
        };
        self.pos = Vertex {
            x: step(p.x, wp.x, wp.x - p.x, 512),
            y: step(p.y, wp.y, wp.y - p.y, 512),
            z: step(p.z, wp.z, wp.z - p.z, 512),
        };
        self.rot = Vertex {
            x: step(r.x, wr.x, angle_diff(wr.x, r.x), 4),
            y: step(r.y, wr.y, angle_diff(wr.y, r.y), 4),
            z: step(r.z, wr.z, angle_diff(wr.z, r.z), 4),
        };
        if done {
            self.easing = false;
        }
    }
}
