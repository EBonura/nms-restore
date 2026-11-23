//! Integer trigonometry and rotation: 4096 angle units to a turn (Escher's,
//! as the game's camera and cue angles), sines from NMS's COSINE.TAB, and
//! matrices in Q12 (4096 = 1.0, the GTE's format). 32-bit integer maths only.

use crate::nms::{cosine, Vertex};

pub type Mat = [[i32; 3]; 3];

/// cos(angle) in 16.16, from COSINE.TAB (E_Init loads it).
pub fn cos16(angle: i32) -> i32 {
    // SAFETY: a 4096-entry table, index masked.
    unsafe { cosine[(angle & 4095) as usize] }
}

pub fn sin16(angle: i32) -> i32 {
    cos16(angle - 1024)
}

fn cs(angle: i32) -> (i32, i32) {
    (cos16(angle) >> 4, sin16(angle) >> 4)
}

/// (r * v) >> 12 for a Q12 factor and any 32-bit v whose product with r
/// stays below 2^43, exactly, with 32-bit multiplies only.
#[inline]
pub fn mul12(r: i32, v: i32) -> i32 {
    r * (v >> 12) + ((r * (v & 0xFFF)) >> 12)
}

pub fn mat_mul(a: &Mat, b: &Mat) -> Mat {
    let mut m = [[0; 3]; 3];
    for i in 0..3 {
        for j in 0..3 {
            m[i][j] = (a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j]) >> 12;
        }
    }
    m
}

/// The rotation Escher's (x, y, z) angles stand for: Ry(y) Rx(x) Rz(z).
/// Rx turns -z toward +y (a camera pitched by x sits above its target,
/// PutCamera; a cue raised by x lifts its butt, PositionCue), Ry turns +z
/// toward +x.  This is a reading of how STV3D.C uses the angles; Escher's
/// own code is lost.
pub fn rotation(x: i32, y: i32, z: i32) -> Mat {
    let (cx, sx) = cs(x);
    let (cy, sy) = cs(y);
    let (cz, sz) = cs(z);
    let rx = [[4096, 0, 0], [0, cx, -sx], [0, sx, cx]];
    let ry = [[cy, 0, sy], [0, 4096, 0], [-sy, 0, cy]];
    let rz = [[cz, -sz, 0], [sz, cz, 0], [0, 0, 4096]];
    mat_mul(&ry, &mat_mul(&rx, &rz))
}

pub fn transpose(m: &Mat) -> Mat {
    [[m[0][0], m[1][0], m[2][0]], [m[0][1], m[1][1], m[2][1]], [m[0][2], m[1][2], m[2][2]]]
}

/// m v for world-sized vectors (|v| below 2^30).
pub fn apply(m: &Mat, v: &Vertex) -> Vertex {
    Vertex {
        x: mul12(m[0][0], v.x) + mul12(m[0][1], v.y) + mul12(m[0][2], v.z),
        y: mul12(m[1][0], v.x) + mul12(m[1][1], v.y) + mul12(m[1][2], v.z),
        z: mul12(m[2][0], v.x) + mul12(m[2][1], v.y) + mul12(m[2][2], v.z),
    }
}
