//! The C library pieces the games call: strings, number parsing, `rand`,
//! formatted output. `memcpy`, `memset` and `memcmp` are psx-rt's (and
//! `memmove` compiler-builtins'), so they are not here.
//!
//! The formatter handles what NMS's format strings use and the usual rest of
//! the integer and string conversions: `%d %i %u %x %X %o %c %s %p %%` with
//! the flags `- 0 + space #`, a width and precision (`*` too) and the `h`/`l`
//! length modifiers. A floating-point conversion prints `?` and says so once
//! on the debug console.

use core::ffi::VaList;

// ------------------------------------------------------------- strings

#[no_mangle]
pub unsafe extern "C" fn strlen(s: *const u8) -> usize {
    let mut n = 0;
    // SAFETY: a NUL-terminated string.
    unsafe {
        while *s.add(n) != 0 {
            n += 1;
        }
    }
    n
}

#[no_mangle]
pub unsafe extern "C" fn strcpy(d: *mut u8, s: *const u8) -> *mut u8 {
    let mut i = 0;
    // SAFETY: a NUL-terminated source and a destination large enough for it.
    unsafe {
        loop {
            let c = *s.add(i);
            *d.add(i) = c;
            if c == 0 {
                break;
            }
            i += 1;
        }
    }
    d
}

#[no_mangle]
pub unsafe extern "C" fn strncpy(d: *mut u8, s: *const u8, n: usize) -> *mut u8 {
    let mut i = 0;
    // SAFETY: as strcpy, at most n bytes; the rest of n is zero-filled.
    unsafe {
        while i < n && *s.add(i) != 0 {
            *d.add(i) = *s.add(i);
            i += 1;
        }
        while i < n {
            *d.add(i) = 0;
            i += 1;
        }
    }
    d
}

#[no_mangle]
pub unsafe extern "C" fn strcat(d: *mut u8, s: *const u8) -> *mut u8 {
    // SAFETY: as strcpy, after the destination's own string.
    unsafe { strcpy(d.add(strlen(d)), s) };
    d
}

#[no_mangle]
pub unsafe extern "C" fn strcmp(a: *const u8, b: *const u8) -> i32 {
    let mut i = 0;
    // SAFETY: two NUL-terminated strings.
    unsafe {
        loop {
            let (x, y) = (*a.add(i), *b.add(i));
            if x != y || x == 0 {
                return x as i32 - y as i32;
            }
            i += 1;
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn strncmp(a: *const u8, b: *const u8, n: usize) -> i32 {
    // SAFETY: two strings, at most n bytes.
    unsafe {
        for i in 0..n {
            let (x, y) = (*a.add(i), *b.add(i));
            if x != y || x == 0 {
                return x as i32 - y as i32;
            }
        }
    }
    0
}

#[no_mangle]
pub unsafe extern "C" fn strchr(s: *const u8, c: i32) -> *mut u8 {
    let mut i = 0;
    // SAFETY: a NUL-terminated string.
    unsafe {
        loop {
            let x = *s.add(i);
            if x == c as u8 {
                return s.add(i) as *mut u8;
            }
            if x == 0 {
                return core::ptr::null_mut();
            }
            i += 1;
        }
    }
}

/// `n` zero bytes from `d` (360X's LOADSAVE.C clears its card buffer so).
#[no_mangle]
pub unsafe extern "C" fn bzero(d: *mut u8, n: usize) {
    // SAFETY: the caller's buffer holds n bytes.
    unsafe { core::ptr::write_bytes(d, 0, n) };
}

#[no_mangle]
pub unsafe extern "C" fn strrchr(s: *const u8, c: i32) -> *mut u8 {
    let mut found = core::ptr::null_mut();
    let mut i = 0;
    // SAFETY: a NUL-terminated string.
    unsafe {
        loop {
            let x = *s.add(i);
            if x == c as u8 {
                found = s.add(i) as *mut u8;
            }
            if x == 0 {
                return found;
            }
            i += 1;
        }
    }
}

// ------------------------------------------------------------------ heap

/// The heap between the program and the stack reserve, handed out in order (8-byte aligned); null
/// when it is used up. 360X takes one block at boot (Initialise_Malloc) and runs its own allocator
/// inside it, and never frees, so there is no free list. It is the SDK's heap (`psx_rt::heap`,
/// `__heap_start..__heap_end` seeded by the linker); a request of 0 bytes gets a block, where this
/// layer's own allocator refused it, and no caller asks for one.
#[no_mangle]
pub extern "C" fn malloc(size: usize) -> *mut u8 {
    psx_rt::heap::alloc_raw(size, 8)
}

#[no_mangle]
pub extern "C" fn free(_p: *mut u8) {
    stub!("free (the heap only grows)");
}

// -------------------------------------------------------------- stdlib

#[no_mangle]
pub extern "C" fn abs(x: i32) -> i32 {
    x.wrapping_abs()
}

#[no_mangle]
pub extern "C" fn labs(x: i32) -> i32 {
    x.wrapping_abs()
}

/// 0 until srand: NMS's 1996 builds keep the seed in .bss (360X's MAIN.SYM
/// names it `n`, after the image), so their first rand() is 12345 >> 16 = 0,
/// not the 1 of the C standard's default seed.
static mut RAND_NEXT: u32 = 0;

/// The classic portable generator: 15 bits of a 32-bit linear congruence.
#[no_mangle]
pub extern "C" fn rand() -> i32 {
    // SAFETY: foreground-only static.
    unsafe {
        let next = psx_fx::LcgRng::step(RAND_NEXT);
        RAND_NEXT = next;
        ((next >> 16) & 0x7FFF) as i32
    }
}

#[no_mangle]
pub extern "C" fn srand(seed: u32) {
    // SAFETY: foreground-only static.
    unsafe { RAND_NEXT = seed };
}

fn is_space(c: u8) -> bool {
    matches!(c, b' ' | b'\t' | b'\n' | b'\r' | 0x0B | 0x0C)
}

#[no_mangle]
pub unsafe extern "C" fn strtol(s: *const u8, end: *mut *mut u8, base: i32) -> i32 {
    // SAFETY: a NUL-terminated string; `end` null or writable.
    unsafe {
        let mut p = s;
        while is_space(*p) {
            p = p.add(1);
        }
        let neg = *p == b'-';
        if *p == b'-' || *p == b'+' {
            p = p.add(1);
        }
        let mut base = base as u32;
        if (base == 0 || base == 16) && *p == b'0' && (*p.add(1) | 0x20) == b'x' {
            p = p.add(2);
            base = 16;
        } else if base == 0 {
            base = if *p == b'0' { 8 } else { 10 };
        }
        let mut v: u32 = 0;
        let start = p;
        loop {
            let c = *p;
            let d = match c {
                b'0'..=b'9' => (c - b'0') as u32,
                b'a'..=b'z' => (c - b'a') as u32 + 10,
                b'A'..=b'Z' => (c - b'A') as u32 + 10,
                _ => 99,
            };
            if d >= base {
                break;
            }
            v = v.wrapping_mul(base).wrapping_add(d);
            p = p.add(1);
        }
        if !end.is_null() {
            *end = if p == start { s as *mut u8 } else { p as *mut u8 };
        }
        if neg { (v as i32).wrapping_neg() } else { v as i32 }
    }
}

#[no_mangle]
pub unsafe extern "C" fn atoi(s: *const u8) -> i32 {
    // SAFETY: forwarded.
    unsafe { strtol(s, core::ptr::null_mut(), 10) }
}

#[no_mangle]
pub unsafe extern "C" fn atol(s: *const u8) -> i32 {
    // SAFETY: forwarded.
    unsafe { strtol(s, core::ptr::null_mut(), 10) }
}

/// 10^n for 0 <= n <= 22, all exact in a double.
fn pow10(n: u32) -> f64 {
    let mut r = 1.0;
    let mut b = 10.0;
    let mut e = n;
    while e > 0 {
        if e & 1 != 0 {
            r *= b;
        }
        b *= b;
        e >>= 1;
    }
    r
}

/// Decimal text to double: the significant digits (up to 19) as an integer,
/// then one multiply or divide by an exact power of ten (in steps of 10^22
/// beyond that), which is within an ulp or two of the nearest double.
#[no_mangle]
pub unsafe extern "C" fn strtod(s: *const u8, end: *mut *mut u8) -> f64 {
    // SAFETY: a NUL-terminated string; `end` null or writable.
    unsafe {
        let mut p = s;
        while is_space(*p) {
            p = p.add(1);
        }
        let neg = *p == b'-';
        if *p == b'-' || *p == b'+' {
            p = p.add(1);
        }
        let mut mant: u64 = 0;
        let mut digits = 0;
        let mut exp10: i32 = 0;
        let mut any = false;
        while (*p).is_ascii_digit() {
            any = true;
            if digits < 19 {
                mant = mant * 10 + (*p - b'0') as u64;
                if mant != 0 {
                    digits += 1;
                }
            } else {
                exp10 += 1;
            }
            p = p.add(1);
        }
        if *p == b'.' {
            p = p.add(1);
            while (*p).is_ascii_digit() {
                any = true;
                if digits < 19 {
                    mant = mant * 10 + (*p - b'0') as u64;
                    if mant != 0 {
                        digits += 1;
                    }
                    exp10 -= 1;
                }
                p = p.add(1);
            }
        }
        if any && (*p | 0x20) == b'e' {
            let mut q = p.add(1);
            let eneg = *q == b'-';
            if *q == b'-' || *q == b'+' {
                q = q.add(1);
            }
            if (*q).is_ascii_digit() {
                let mut e: i32 = 0;
                while (*q).is_ascii_digit() {
                    e = (e * 10 + (*q - b'0') as i32).min(10_000);
                    q = q.add(1);
                }
                exp10 += if eneg { -e } else { e };
                p = q;
            }
        }
        if !end.is_null() {
            *end = if any { p as *mut u8 } else { s as *mut u8 };
        }
        let mut v = mant as f64;
        while exp10 > 22 {
            v *= pow10(22);
            exp10 -= 22;
        }
        while exp10 < -22 {
            v /= pow10(22);
            exp10 += 22;
        }
        if exp10 >= 0 {
            v *= pow10(exp10 as u32);
        } else {
            v /= pow10((-exp10) as u32);
        }
        if neg { -v } else { v }
    }
}

#[no_mangle]
pub unsafe extern "C" fn atof(s: *const u8) -> f64 {
    // SAFETY: forwarded.
    unsafe { strtod(s, core::ptr::null_mut()) }
}

/// `value` in `radix` (2..36), negative only in base 10.
#[no_mangle]
pub unsafe extern "C" fn itoa(value: i32, buf: *mut u8, radix: i32) -> *mut u8 {
    let radix = if (2..=36).contains(&radix) { radix as u32 } else { 10 };
    let mut tmp = [0u8; 34];
    let mut n = 0;
    let neg = radix == 10 && value < 0;
    let mut u = if neg { value.unsigned_abs() } else { value as u32 };
    loop {
        let d = (u % radix) as u8;
        tmp[n] = if d < 10 { b'0' + d } else { b'a' + d - 10 };
        n += 1;
        u /= radix;
        if u == 0 {
            break;
        }
    }
    let mut i = 0;
    // SAFETY: the caller's buffer holds the digits, a sign and the NUL.
    unsafe {
        if neg {
            *buf = b'-';
            i = 1;
        }
        while n > 0 {
            n -= 1;
            *buf.add(i) = tmp[n];
            i += 1;
        }
        *buf.add(i) = 0;
    }
    buf
}

// ------------------------------------------------------ formatted output

/// Format `fmt` with the C arguments in `ap`, one byte at a time to `out`.
pub unsafe fn format(fmt: *const u8, ap: &mut VaList<'_>, out: &mut dyn FnMut(u8)) {
    let mut p = fmt;
    // SAFETY: a NUL-terminated format string whose conversions match the
    // arguments the caller passed (as C requires).
    unsafe {
        loop {
            let c = *p;
            if c == 0 {
                return;
            }
            p = p.add(1);
            if c != b'%' {
                out(c);
                continue;
            }
            let (mut left, mut zero, mut plus, mut space, mut alt) = (false, false, false, false, false);
            loop {
                match *p {
                    b'-' => left = true,
                    b'0' => zero = true,
                    b'+' => plus = true,
                    b' ' => space = true,
                    b'#' => alt = true,
                    _ => break,
                }
                p = p.add(1);
            }
            let mut width: i32 = 0;
            if *p == b'*' {
                width = ap.arg::<i32>();
                if width < 0 {
                    left = true;
                    width = -width;
                }
                p = p.add(1);
            } else {
                while (*p).is_ascii_digit() {
                    width = width * 10 + (*p - b'0') as i32;
                    p = p.add(1);
                }
            }
            let mut prec: i32 = -1;
            if *p == b'.' {
                p = p.add(1);
                prec = 0;
                if *p == b'*' {
                    prec = ap.arg::<i32>().max(-1);
                    p = p.add(1);
                } else {
                    while (*p).is_ascii_digit() {
                        prec = prec * 10 + (*p - b'0') as i32;
                        p = p.add(1);
                    }
                }
            }
            while matches!(*p, b'h' | b'l' | b'L') {
                p = p.add(1);
            }
            let conv = *p;
            if conv == 0 {
                return;
            }
            p = p.add(1);
            let mut digits = [0u8; 40];
            let mut nd = 0;
            let mut prefix: &[u8] = b"";
            match conv {
                b'd' | b'i' | b'u' | b'x' | b'X' | b'o' | b'p' => {
                    let (neg, mut u) = match conv {
                        b'd' | b'i' => {
                            let v = ap.arg::<i32>();
                            (v < 0, v.unsigned_abs())
                        }
                        b'p' => (false, ap.arg::<*const u8>() as u32),
                        _ => (false, ap.arg::<u32>()),
                    };
                    let base = match conv {
                        b'x' | b'X' | b'p' => 16,
                        b'o' => 8,
                        _ => 10,
                    };
                    let upper = conv == b'X';
                    while u != 0 {
                        let d = (u % base) as u8;
                        digits[nd] = if d < 10 { b'0' + d } else if upper { b'A' + d - 10 } else { b'a' + d - 10 };
                        nd += 1;
                        u /= base;
                    }
                    let min_digits = if prec >= 0 { prec as usize } else { 1 };
                    while nd < min_digits && nd < digits.len() {
                        digits[nd] = b'0';
                        nd += 1;
                    }
                    prefix = if neg {
                        b"-"
                    } else if plus && matches!(conv, b'd' | b'i') {
                        b"+"
                    } else if space && matches!(conv, b'd' | b'i') {
                        b" "
                    } else if (alt && matches!(conv, b'x' | b'X')) || conv == b'p' {
                        if upper { b"0X" } else { b"0x" }
                    } else {
                        b""
                    };
                    let zero = zero && !left && prec < 0;
                    let len = prefix.len() + nd;
                    let pad = (width as usize).saturating_sub(len);
                    if !left && !zero {
                        (0..pad).for_each(|_| out(b' '));
                    }
                    prefix.iter().for_each(|&b| out(b));
                    if zero {
                        (0..pad).for_each(|_| out(b'0'));
                    }
                    while nd > 0 {
                        nd -= 1;
                        out(digits[nd]);
                    }
                    if left {
                        (0..pad).for_each(|_| out(b' '));
                    }
                }
                b'c' | b's' => {
                    let ch: [u8; 1];
                    let (sp, len): (*const u8, usize) = if conv == b'c' {
                        ch = [ap.arg::<i32>() as u8];
                        (ch.as_ptr(), 1)
                    } else {
                        let s = ap.arg::<*const u8>();
                        let s = if s.is_null() { b"(null)\0".as_ptr() } else { s };
                        let mut n = 0;
                        while *s.add(n) != 0 && (prec < 0 || n < prec as usize) {
                            n += 1;
                        }
                        (s, n)
                    };
                    let pad = (width as usize).saturating_sub(len);
                    if !left {
                        (0..pad).for_each(|_| out(b' '));
                    }
                    for i in 0..len {
                        out(*sp.add(i));
                    }
                    if left {
                        (0..pad).for_each(|_| out(b' '));
                    }
                }
                b'%' => out(b'%'),
                b'f' | b'F' | b'e' | b'E' | b'g' | b'G' => {
                    let _ = ap.arg::<f64>();
                    stub!("printf floating-point conversion");
                    out(b'?');
                }
                _ => out(b'?'),
            }
            let _ = prefix;
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn sprintf(buf: *mut u8, fmt: *const u8, mut args: ...) -> i32 {
    let mut n = 0usize;
    // SAFETY: the caller's buffer is large enough for the result (as C requires).
    unsafe {
        format(fmt, &mut args, &mut |c| {
            *buf.add(n) = c;
            n += 1;
        });
        *buf.add(n) = 0;
    }
    n as i32
}

#[no_mangle]
pub unsafe extern "C" fn printf(fmt: *const u8, mut args: ...) -> i32 {
    // RESTORE: PLATFORM (owner's decision, 2026-11-13): with the console off there is nothing to
    // format into, so the call costs nothing (see console::putc).
    if !cfg!(feature = "debug-console") {
        return 0;
    }
    let mut n = 0;
    // SAFETY: forwarded.
    unsafe {
        format(fmt, &mut args, &mut |c| {
            crate::console::putc(c);
            n += 1;
        });
    }
    n
}

#[no_mangle]
pub unsafe extern "C" fn puts(s: *const u8) -> i32 {
    // RESTORE: PLATFORM (owner's decision, 2026-11-13): as printf.
    if !cfg!(feature = "debug-console") {
        return 1;
    }
    // SAFETY: a NUL-terminated string.
    unsafe {
        let mut p = s;
        while *p != 0 {
            crate::console::putc(*p);
            p = p.add(1);
        }
    }
    crate::console::putc(b'\n');
    1
}

#[no_mangle]
pub extern "C" fn putchar(c: i32) -> i32 {
    crate::console::putc(c as u8);
    c
}

