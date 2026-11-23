//! Geometry (libgte): GTE set-up and the 4.12 fixed-point matrix maths the
//! graphics library and NMS's planet screens use.
//!
//! The arithmetic follows NMS's 1996 build of 360X, read from its MAIN.CPE
//! (MAIN.SYM names the functions): every product of two 4.12 numbers is
//! shifted right by 12 (an arithmetic shift, so it rounds down), and the
//! routines that ran on the GTE (`MulMatrix`, `ApplyMatrixLV`) round the way
//! the GTE does, which is the same shift. `ApplyMatrixLV` splits a long
//! vector into a high and a low 15-bit part and shifts each product
//! separately, as the 1996 build does, so its small rounding error is kept.
//!
//! The sine table is this crate's own (`round(4096 * sin)` on a quarter turn,
//! built at compile time); the 1996 `rsin`/`rcos` quarter-wave table has the
//! same values. The full-turn table the 1996 `RotMatrix` reads is that
//! quarter sine with the second and fourth quarters mirrored one step off:
//! sin(1024 + j) is taken as sin(1023 - j) (so its cosine is sin(1023 - j),
//! up to 7/4096 low near 45 degrees). `rot_sin` reproduces that rule; with
//! it all 4096 entries equal the 1996 table's, checked against MAIN.CPE.

/// Psy-Q's MATRIX: a 3x3 rotation in 4.12 and a translation.
#[repr(C)]
#[derive(Clone, Copy)]
pub struct Matrix {
    pub m: [[i16; 3]; 3],
    pub t: [i32; 3],
}

/// Psy-Q's VECTOR (32-bit components).
#[repr(C)]
#[derive(Clone, Copy)]
pub struct Vector {
    pub vx: i32,
    pub vy: i32,
    pub vz: i32,
    pub pad: i32,
}

/// Psy-Q's SVECTOR (16-bit components).
#[repr(C)]
#[derive(Clone, Copy)]
pub struct SVector {
    pub vx: i16,
    pub vy: i16,
    pub vz: i16,
    pub pad: i16,
}

pub const IDENTITY: Matrix = Matrix { m: [[4096, 0, 0], [0, 4096, 0], [0, 0, 4096]], t: [0; 3] };
pub const ZERO: Matrix = Matrix { m: [[0; 3]; 3], t: [0; 3] };

// ------------------------------------------------------------ GTE access

/// One GTE function command, as the word the 1996 build used (the GTE reads
/// only sf, lm, the MVMVA fields and the function number of it).
macro_rules! gte_cmd {
    ($word:expr) => {{
        #[cfg(target_arch = "mips")]
        // SAFETY: a COP2 function command reads and writes GTE registers only.
        #[allow(unused_unsafe)]
        unsafe {
            core::arch::asm!("nop", "nop", ".word {w}", w = const $word, options(nostack, preserves_flags))
        }
    }};
}
pub(crate) use gte_cmd;

// The SDK's command words. The 1996 build's words differed from these only in bits the GTE
// ignores (bits 24..20, the "fake" command number), and in `sf` on AVSZ3, which that command
// does not read; the function number, `sf`, `lm` and the MVMVA selects are the same.
use psx_hw::gte::command;

pub const RTPS: u32 = command::RTPS;
pub const RTPT: u32 = command::RTPT;
pub const NCLIP: u32 = command::NCLIP;
pub const AVSZ3: u32 = command::AVSZ3;
pub const NCDS: u32 = command::NCDS_LM;
pub const NCCS: u32 = command::NCCS_LM;

#[inline(always)]
fn pack(lo: i16, hi: i16) -> u32 {
    (lo as u16 as u32) | ((hi as u16 as u32) << 16)
}

pub fn set_rot_matrix(m: &Matrix) {
    psx_gte::write_control!(0, pack(m.m[0][0], m.m[0][1]));
    psx_gte::write_control!(1, pack(m.m[0][2], m.m[1][0]));
    psx_gte::write_control!(2, pack(m.m[1][1], m.m[1][2]));
    psx_gte::write_control!(3, pack(m.m[2][0], m.m[2][1]));
    psx_gte::write_control!(4, m.m[2][2] as u16 as u32);
}

pub fn set_trans_matrix(m: &Matrix) {
    psx_gte::write_control!(5, m.t[0] as u32);
    psx_gte::write_control!(6, m.t[1] as u32);
    psx_gte::write_control!(7, m.t[2] as u32);
}

pub fn set_light_matrix(m: &Matrix) {
    psx_gte::write_control!(8, pack(m.m[0][0], m.m[0][1]));
    psx_gte::write_control!(9, pack(m.m[0][2], m.m[1][0]));
    psx_gte::write_control!(10, pack(m.m[1][1], m.m[1][2]));
    psx_gte::write_control!(11, pack(m.m[2][0], m.m[2][1]));
    psx_gte::write_control!(12, m.m[2][2] as u16 as u32);
}

pub fn set_color_matrix(m: &Matrix) {
    psx_gte::write_control!(16, pack(m.m[0][0], m.m[0][1]));
    psx_gte::write_control!(17, pack(m.m[0][2], m.m[1][0]));
    psx_gte::write_control!(18, pack(m.m[1][1], m.m[1][2]));
    psx_gte::write_control!(19, pack(m.m[2][0], m.m[2][1]));
    psx_gte::write_control!(20, m.m[2][2] as u16 as u32);
}

pub fn read_geom_screen() -> i32 {
    psx_gte::read_control!(26) as i32
}

// ------------------------------------------------------------ set-up calls

/// The GTE defaults the 1996 build's InitGeom loads: ZSF3 = 341 and
/// ZSF4 = 256 (average Z / 4), projection 1000, DQA = -4194, DQB = 0x1400000,
/// offset 0.
#[no_mangle]
pub extern "C" fn InitGeom() {
    psx_gte::write_control!(29, 341);
    psx_gte::write_control!(30, 256);
    psx_gte::write_control!(26, 1000);
    psx_gte::write_control!(27, (-4194i32) as u32);
    psx_gte::write_control!(28, 0x0140_0000);
    psx_gte::write_control!(24, 0);
    psx_gte::write_control!(25, 0);
}

#[no_mangle]
pub extern "C" fn SetGeomOffset(ofx: i32, ofy: i32) {
    psx_gte::write_control!(24, (ofx << 16) as u32);
    psx_gte::write_control!(25, (ofy << 16) as u32);
}

#[no_mangle]
pub extern "C" fn SetGeomScreen(h: i32) {
    psx_gte::write_control!(26, h as u32);
}

#[no_mangle]
pub extern "C" fn ReadGeomScreen() -> i32 {
    read_geom_screen()
}

/// Ambient colour: the GTE's BK registers hold the argument times 16.
#[no_mangle]
pub extern "C" fn SetBackColor(r: i32, g: i32, b: i32) {
    psx_gte::write_control!(13, (r << 4) as u32);
    psx_gte::write_control!(14, (g << 4) as u32);
    psx_gte::write_control!(15, (b << 4) as u32);
}

/// Fog colour: the GTE's FC registers hold the argument times 16.
#[no_mangle]
pub extern "C" fn SetFarColor(r: i32, g: i32, b: i32) {
    psx_gte::write_control!(21, (r << 4) as u32);
    psx_gte::write_control!(22, (g << 4) as u32);
    psx_gte::write_control!(23, (b << 4) as u32);
}

#[no_mangle]
pub extern "C" fn SetDQA(a: i32) {
    psx_gte::write_control!(27, a as u32);
}

#[no_mangle]
pub extern "C" fn SetDQB(b: i32) {
    psx_gte::write_control!(28, b as u32);
}

#[no_mangle]
pub unsafe extern "C" fn SetRotMatrix(m: *const Matrix) {
    // SAFETY: the game's MATRIX.
    set_rot_matrix(unsafe { &*m })
}

#[no_mangle]
pub unsafe extern "C" fn SetTransMatrix(m: *const Matrix) {
    // SAFETY: the game's MATRIX.
    set_trans_matrix(unsafe { &*m })
}

#[no_mangle]
pub unsafe extern "C" fn SetLightMatrix(m: *const Matrix) {
    // SAFETY: the game's MATRIX.
    set_light_matrix(unsafe { &*m })
}

#[no_mangle]
pub unsafe extern "C" fn SetColorMatrix(m: *const Matrix) {
    // SAFETY: the game's MATRIX.
    set_color_matrix(unsafe { &*m })
}

// ------------------------------------------------------------ the maths

/// sin(2 pi i / 4096) in 4.12, rounded to nearest, for i in 0..=1024.
static QUARTER_SINE: [i16; 1025] = {
    // A degree-11 odd Taylor series in fixed point, evaluated at compile
    // time with 64-bit integers, is accurate to far better than 1/4096 on
    // a quarter turn; the table is then rounded to nearest.
    let mut t = [0i16; 1025];
    let mut i = 0;
    while i <= 1024 {
        // x = 2 pi i / 4096 in Q40
        const PI_Q40: i128 = 3_454_217_652_358; // pi * 2^40
        let x: i128 = 2 * PI_Q40 * i as i128 / 4096;
        let x2 = x * x >> 40;
        let mut term = x; // x^1/1!
        let mut sum = x;
        let mut k = 1;
        while k <= 6 {
            term = -term * x2 >> 40;
            term /= ((2 * k) * (2 * k + 1)) as i128;
            sum += term;
            k += 1;
        }
        // sum * 4096 rounded
        let v = (sum * 4096 + (1i128 << 39)) >> 40;
        t[i] = v as i16;
        i += 1;
    }
    t
};

/// Sine of a 4096-per-turn angle, in 4.12.
pub fn rsin(a: i32) -> i32 {
    let neg = a < 0;
    let a = (if neg { a.wrapping_neg() } else { a }) & 0xFFF;
    let s = match a {
        0..=1024 => QUARTER_SINE[a as usize] as i32,
        1025..=2048 => QUARTER_SINE[(2048 - a) as usize] as i32,
        2049..=3072 => -(QUARTER_SINE[(a - 2048) as usize] as i32),
        _ => -(QUARTER_SINE[(4096 - a) as usize] as i32),
    };
    if neg { -s } else { s }
}

/// The sine `RotMatrix` uses (see the module comment), angle 0..4095.
fn rot_sin(a: i32) -> i32 {
    let a = a & 0xFFF;
    match a {
        0..=1023 => QUARTER_SINE[a as usize] as i32,
        1024..=2047 => QUARTER_SINE[(2047 - a) as usize] as i32,
        2048..=3071 => -(QUARTER_SINE[(a - 2048) as usize] as i32),
        _ => -(QUARTER_SINE[(4095 - a) as usize] as i32),
    }
}

pub fn rcos(a: i32) -> i32 {
    rsin((a & 0xFFF) + 1024)
}

/// floor(512 * sqrt(n)) for n in 64..=255: SquareRoot0's mantissa table.
static SQRT_MANTISSA: [u16; 192] = {
    let mut t = [0u16; 192];
    let mut i = 0;
    while i < 192 {
        // integer square root of (i + 64) * 512^2
        let n = (i as u32 + 64) * 262_144;
        let mut x = 8192u32;
        while x * x > n {
            x -= 1;
        }
        t[i] = x as u16;
        i += 1;
    }
    t
};

/// The square root as the 1996 build's SquareRoot0 works it out, which is
/// not exact: the argument shifted by an even count to 8 significant bits
/// (64..255), a mantissa from a table of floor(512 sqrt(m)), shifted back
/// by half the count. sqrt(25000000) comes out 4990, not 5000; the view and
/// light directions of GsSetRefView2 and GsSetFlatLight carry that.
pub fn square_root0(a: i32) -> i32 {
    if a == 0 {
        return 0;
    }
    let lz = (a as u32).leading_zeros() as i32;
    let even = lz & !1;
    let up = (31 - even) >> 1;
    let m = if even >= 24 { (a as u32) << (even - 24) } else { ((a as i32) >> (24 - even)) as u32 };
    let Some(&mant) = SQRT_MANTISSA.get((m as usize).wrapping_sub(64)) else { return 0 };
    let mant = mant as u32;
    ((mant << up) >> 12) as i32
}

#[no_mangle]
pub extern "C" fn SquareRoot0(a: i32) -> i32 {
    square_root0(a)
}

#[inline(always)]
fn fx(a: i32, b: i32) -> i32 {
    a.wrapping_mul(b) >> 12
}

/// `a * b` (rotation parts), each element shifted down by 12 as the GTE does.
pub fn mul_rot(a: &Matrix, b: &Matrix) -> [[i16; 3]; 3] {
    let mut r = [[0i16; 3]; 3];
    for i in 0..3 {
        for j in 0..3 {
            let s: i64 = (0..3).map(|k| a.m[i][k] as i64 * b.m[k][j] as i64).sum();
            r[i][j] = (s >> 12) as i16;
        }
    }
    r
}

/// The 1996 ApplyMatrixLV: `m * v` for a 32-bit vector, done in two GTE
/// passes on the high and low 15 bits of each component (sign kept apart).
pub fn apply_lv(m: &Matrix, v: [i32; 3]) -> [i32; 3] {
    let split = |x: i32| -> (i32, i32) {
        let mag = x.unsigned_abs();
        let (hi, lo) = ((mag >> 15) as i32, (mag & 0x7FFF) as i32);
        if x < 0 { (-hi, -lo) } else { (hi, lo) }
    };
    let (h, l): (Vec3, Vec3) = {
        let s = [split(v[0]), split(v[1]), split(v[2])];
        ([s[0].0, s[1].0, s[2].0], [s[0].1, s[1].1, s[2].1])
    };
    let mut out = [0; 3];
    for i in 0..3 {
        let hs: i64 = (0..3).map(|k| m.m[i][k] as i64 * h[k] as i64).sum();
        let ls: i64 = (0..3).map(|k| m.m[i][k] as i64 * l[k] as i64).sum();
        out[i] = ((ls >> 12) as i32).wrapping_add(((hs >> 12) as i32).wrapping_mul(8));
    }
    out
}

type Vec3 = [i32; 3];

/// a = a ∘ b (GsMulCoord3): rotation a*b, translation a.rot*b.t + a.t.
pub fn compose(a: &Matrix, b: &Matrix) -> Matrix {
    let v = apply_lv(a, b.t);
    Matrix { m: mul_rot(a, b), t: [v[0].wrapping_add(a.t[0]), v[1].wrapping_add(a.t[1]), v[2].wrapping_add(a.t[2])] }
}

/// m0 = m0 * m1 (rotation only; m0's translation is kept).
#[no_mangle]
pub unsafe extern "C" fn MulMatrix(m0: *mut Matrix, m1: *const Matrix) -> *mut Matrix {
    // SAFETY: the caller's matrices.
    unsafe { (*m0).m = mul_rot(&*m0, &*m1) };
    m0
}

/// m1 = m0 * m1 (rotation only; m1's translation is kept).
#[no_mangle]
pub unsafe extern "C" fn MulMatrix2(m0: *const Matrix, m1: *mut Matrix) -> *mut Matrix {
    // SAFETY: the caller's matrices.
    unsafe { (*m1).m = mul_rot(&*m0, &*m1) };
    m1
}

#[no_mangle]
pub unsafe extern "C" fn ApplyMatrixLV(m: *const Matrix, v: *const Vector, out: *mut Vector) -> *mut Vector {
    // SAFETY: the caller's matrix and vectors.
    unsafe {
        let r = apply_lv(&*m, [(*v).vx, (*v).vy, (*v).vz]);
        (*out).vx = r[0];
        (*out).vy = r[1];
        (*out).vz = r[2];
    }
    out
}

#[no_mangle]
pub unsafe extern "C" fn TransMatrix(m: *mut Matrix, v: *const Vector) -> *mut Matrix {
    // SAFETY: the caller's matrix and vector.
    unsafe { (*m).t = [(*v).vx, (*v).vy, (*v).vz] };
    m
}

/// Column j scaled by v[j] (4.12), each product shifted down by 12 and
/// kept to 16 bits, as the 1996 build does.
#[no_mangle]
pub unsafe extern "C" fn ScaleMatrix(m: *mut Matrix, v: *const Vector) -> *mut Matrix {
    // SAFETY: the caller's matrix and vector.
    unsafe {
        let s = [(*v).vx, (*v).vy, (*v).vz];
        for i in 0..3 {
            for j in 0..3 {
                (*m).m[i][j] = fx((*m).m[i][j] as i32, s[j]) as i16;
            }
        }
    }
    m
}

/// The rotation for angles (vx, vy, vz), 4096 to a turn, with the 1996
/// build's operation order and roundings: entry by entry, sines and cosines
/// of x, y and z combined two at a time, each product shifted down by 12.
#[no_mangle]
pub unsafe extern "C" fn RotMatrix(r: *const SVector, m: *mut Matrix) -> *mut Matrix {
    // SAFETY: the caller's angles and matrix.
    let (r, out) = unsafe { (&*r, &mut *m) };
    // sine and cosine of an angle; a negative angle by symmetry, as in 1996
    let sc = |a: i16| {
        let m = (a as i32).abs() & 0xFFF;
        let (sn, cs) = (rot_sin(m), rot_sin(m + 1024));
        if a < 0 { (-sn, cs) } else { (sn, cs) }
    };
    let (sx, cx) = sc(r.vx);
    let (sy, cy) = sc(r.vy);
    let (sz, cz) = sc(r.vz);
    let nsy = -sy;
    out.m[0][2] = sy as i16;
    out.m[1][2] = (cy.wrapping_mul(sx).wrapping_neg() >> 12) as i16;
    out.m[2][2] = fx(cy, cx) as i16;
    out.m[0][0] = fx(cz, cy) as i16;
    out.m[0][1] = (sz.wrapping_mul(cy).wrapping_neg() >> 12) as i16;
    let t = fx(cz, nsy);
    out.m[1][0] = (fx(sz, cx) - fx(t, sx)) as i16;
    out.m[2][0] = (fx(sz, sx) + fx(t, cx)) as i16;
    let t = fx(sz, nsy);
    out.m[1][1] = (fx(cz, cx) + fx(t, sx)) as i16;
    out.m[2][1] = (fx(cz, sx) - fx(t, cx)) as i16;
    m
}

#[no_mangle]
pub unsafe extern "C" fn TransposeMatrix(m0: *const Matrix, m1: *mut Matrix) -> *mut Matrix {
    // SAFETY: the caller's matrices.
    unsafe {
        let a = *m0;
        for i in 0..3 {
            for j in 0..3 {
                (*m1).m[i][j] = a.m[j][i];
            }
        }
    }
    m1
}
