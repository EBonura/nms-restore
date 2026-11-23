//! The debug console (expansion 2, 0x1F802080): what `printf` writes and
//! where stubs report themselves. The PSoXide emulator prints it on the host
//! terminal (`--stop-on-tty` matches on it).

#[cfg(feature = "debug-console")]
use psx_hw::memory::expansion2::DEBUG_CONSOLE;

/// One byte to the debug console.
///
/// RESTORE: PLATFORM (owner's decision, 2026-11-13: "gate the debug printout"). Every write to
/// the console, NMS's `printf` lines and the layer's own, comes through here, so a build without
/// the `debug-console` feature (the disc builds) prints nothing. The headless tools that read the
/// console build with `PSYQ_DEBUG=1`.
#[inline]
pub fn putc(c: u8) {
    #[cfg(feature = "debug-console")]
    // SAFETY: the debug console is a byte-wide write-only port; through KSEG1
    // the store is uncached and reaches it at once.
    unsafe {
        core::ptr::write_volatile((DEBUG_CONSOLE | 0xA000_0000) as *mut u8, c)
    }
    #[cfg(not(feature = "debug-console"))]
    let _ = c;
}

pub fn puts(s: &str) {
    for &c in s.as_bytes() {
        putc(c);
    }
}

/// Report, once per call site, that the game reached something this layer
/// does not implement.
macro_rules! stub {
    ($name:expr) => {{
        static mut SEEN: bool = false;
        // SAFETY: a private flag, only touched here, from the foreground.
        #[allow(unused_unsafe)]
        unsafe {
            if !core::ptr::read_volatile(&raw const SEEN) {
                core::ptr::write_volatile(&raw mut SEEN, true);
                $crate::console::puts("psyq: STUB ");
                $crate::console::puts($name);
                $crate::console::puts(" (not implemented)\n");
            }
        }
    }};
}
