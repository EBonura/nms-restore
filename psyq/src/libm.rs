//! Double-precision maths for the games' floating-point code. The arithmetic
//! is the compiler's soft-float; the functions are written here:
//!
//! * `sqrt`: Newton's iteration from an exponent-halved first guess.
//! * `sin`, `cos`, `tan`: reduction by multiples of pi/2 (pi/2 in three parts,
//!   Cody and Waite), then Taylor series on [-pi/4, pi/4], whose truncation
//!   error there is below 1e-17.
//! * `atan`: |x| > 1 through pi/2 - atan(1/x), |x| > tan(pi/12) through
//!   pi/6 + atan((x*sqrt(3) - 1)/(x + sqrt(3))), then the series to x^27.
//! * `atan2`, `asin`, `acos`, `hypot`, `fabs`, `floor`, `ceil`, `fmod` from
//!   those.
//!
//! Accuracy is a few ulp; arguments past about 1e9 radians lose precision in
//! the reduction. NMS's code works with table-sized values (metres, angles in
//! one or two turns), far inside that.

const PI: f64 = 3.141_592_653_589_793;
const PI_2: f64 = 1.570_796_326_794_896_6;
const PI_6: f64 = 0.523_598_775_598_298_8;
const SQRT3: f64 = 1.732_050_807_568_877_2;
const TAN_PI_12: f64 = 0.267_949_192_431_122_7;
const TWO_OVER_PI: f64 = 0.636_619_772_367_581_4;
// pi/2 = P1 + P2 + P3, P1 and P2 with trailing zero bits so k*P1 and k*P2 are exact.
const P1: f64 = 1.570_796_326_734_125_6;
const P2: f64 = 6.077_100_506_303_966e-11;
const P3: f64 = 2.022_266_248_795_950_6e-21;

#[no_mangle]
pub extern "C" fn fabs(x: f64) -> f64 {
    f64::from_bits(x.to_bits() & !(1u64 << 63))
}

fn is_nan(x: f64) -> bool {
    x != x
}

#[no_mangle]
pub extern "C" fn sqrt(x: f64) -> f64 {
    if is_nan(x) || x < 0.0 {
        return f64::NAN;
    }
    if x == 0.0 || x == f64::INFINITY {
        return x;
    }
    // halve the exponent for a first guess within about 6%
    let mut y = f64::from_bits((x.to_bits() >> 1) + (1023u64 << 51));
    for _ in 0..5 {
        y = 0.5 * (y + x / y);
    }
    y
}

#[no_mangle]
pub extern "C" fn floor(x: f64) -> f64 {
    if is_nan(x) || fabs(x) >= 4_503_599_627_370_496.0 {
        return x;
    }
    let t = x as i64 as f64;
    if t > x { t - 1.0 } else { t }
}

#[no_mangle]
pub extern "C" fn ceil(x: f64) -> f64 {
    -floor(-x)
}

#[no_mangle]
pub extern "C" fn fmod(x: f64, y: f64) -> f64 {
    if y == 0.0 || is_nan(x) || is_nan(y) {
        return f64::NAN;
    }
    let q = x / y;
    let t = if q < 0.0 { ceil(q) } else { floor(q) };
    x - t * y
}

/// sin(r) for |r| <= pi/4.
fn sin_kernel(r: f64) -> f64 {
    let r2 = r * r;
    // x - x^3/3! + x^5/5! ... - x^15/15!
    let mut s = -1.0 / 1_307_674_368_000.0;
    s = s * r2 + 1.0 / 6_227_020_800.0;
    s = s * r2 - 1.0 / 39_916_800.0;
    s = s * r2 + 1.0 / 362_880.0;
    s = s * r2 - 1.0 / 5040.0;
    s = s * r2 + 1.0 / 120.0;
    s = s * r2 - 1.0 / 6.0;
    r + r * r2 * s
}

/// cos(r) for |r| <= pi/4.
fn cos_kernel(r: f64) -> f64 {
    let r2 = r * r;
    // 1 - x^2/2! + x^4/4! ... + x^16/16!
    let mut c = 1.0 / 20_922_789_888_000.0;
    c = c * r2 - 1.0 / 87_178_291_200.0;
    c = c * r2 + 1.0 / 479_001_600.0;
    c = c * r2 - 1.0 / 3_628_800.0;
    c = c * r2 + 1.0 / 40_320.0;
    c = c * r2 - 1.0 / 720.0;
    c = c * r2 + 1.0 / 24.0;
    c = c * r2 - 0.5;
    1.0 + r2 * c
}

/// x = k*pi/2 + r with |r| <= pi/4; returns (k mod 4, r).
fn reduce(x: f64) -> (i32, f64) {
    if fabs(x) <= PI / 4.0 {
        return (0, x);
    }
    let kf = floor(x * TWO_OVER_PI + 0.5);
    let r = ((x - kf * P1) - kf * P2) - kf * P3;
    ((kf as i64 & 3) as i32, r)
}

#[no_mangle]
pub extern "C" fn sin(x: f64) -> f64 {
    if is_nan(x) || fabs(x) == f64::INFINITY {
        return f64::NAN;
    }
    let (k, r) = reduce(x);
    match k {
        0 => sin_kernel(r),
        1 => cos_kernel(r),
        2 => -sin_kernel(r),
        _ => -cos_kernel(r),
    }
}

#[no_mangle]
pub extern "C" fn cos(x: f64) -> f64 {
    if is_nan(x) || fabs(x) == f64::INFINITY {
        return f64::NAN;
    }
    let (k, r) = reduce(x);
    match k {
        0 => cos_kernel(r),
        1 => -sin_kernel(r),
        2 => -cos_kernel(r),
        _ => sin_kernel(r),
    }
}

#[no_mangle]
pub extern "C" fn tan(x: f64) -> f64 {
    sin(x) / cos(x)
}

/// atan(t) for |t| <= tan(pi/12).
fn atan_kernel(t: f64) -> f64 {
    let t2 = t * t;
    let mut s = 0.0;
    let mut n = 27i32;
    while n >= 3 {
        let term = 1.0 / n as f64;
        s = if (n / 2) % 2 == 1 { -term } else { term } + t2 * s;
        n -= 2;
    }
    t + t * t2 * s
}

#[no_mangle]
pub extern "C" fn atan(x: f64) -> f64 {
    if is_nan(x) {
        return x;
    }
    let neg = x < 0.0;
    let mut a = fabs(x);
    let mut base = 0.0;
    let mut flip = false;
    if a > 1.0 {
        a = 1.0 / a;
        flip = true;
    }
    if a > TAN_PI_12 {
        a = (a * SQRT3 - 1.0) / (a + SQRT3);
        base = PI_6;
    }
    let mut r = base + atan_kernel(a);
    if flip {
        r = PI_2 - r;
    }
    if neg { -r } else { r }
}

#[no_mangle]
pub extern "C" fn atan2(y: f64, x: f64) -> f64 {
    if is_nan(x) || is_nan(y) {
        return f64::NAN;
    }
    if x == 0.0 {
        if y > 0.0 {
            return PI_2;
        }
        if y < 0.0 {
            return -PI_2;
        }
        // atan2(+-0, +0) = +-0, atan2(+-0, -0) = +-pi
        let y_neg = y.to_bits() >> 63 != 0;
        if x.to_bits() >> 63 != 0 {
            return if y_neg { -PI } else { PI };
        }
        return y;
    }
    let a = atan(y / x);
    if x > 0.0 {
        a
    } else if y.to_bits() >> 63 != 0 {
        a - PI
    } else {
        a + PI
    }
}

#[no_mangle]
pub extern "C" fn asin(x: f64) -> f64 {
    if fabs(x) > 1.0 {
        return f64::NAN;
    }
    atan2(x, sqrt((1.0 - x) * (1.0 + x)))
}

#[no_mangle]
pub extern "C" fn acos(x: f64) -> f64 {
    if fabs(x) > 1.0 {
        return f64::NAN;
    }
    atan2(sqrt((1.0 - x) * (1.0 + x)), x)
}

#[no_mangle]
pub extern "C" fn hypot(x: f64, y: f64) -> f64 {
    let (a, b) = (fabs(x), fabs(y));
    let (big, small) = if a > b { (a, b) } else { (b, a) };
    if big == 0.0 || big == f64::INFINITY {
        return big;
    }
    let r = small / big;
    big * sqrt(1.0 + r * r)
}
