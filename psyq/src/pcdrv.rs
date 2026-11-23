//! The dev kit's PC link: files on the developer's PC, reached through
//! `BREAK` instructions whose code field names the call.
//!
//! The host takes its arguments in `a1`..`a3` (a C wrapper's arguments
//! shifted up one register) and answers in `v0` (0 = done) and `v1` (the
//! result). This is the convention the PSoXide emulator serves with
//! `--pcdrv DIR`, read off NMS's 1996 call sites; on a console without a
//! dev kit the `BREAK` is an exception and every call fails.
//!
//! | code  | call   | a1     | a2        | a3     |
//! |-------|--------|--------|-----------|--------|
//! | 0x101 | init   |        |           |        |
//! | 0x102 | creat  | path   | attribute |        |
//! | 0x103 | open   | path   | mode      |        |
//! | 0x104 | close  | handle |           |        |
//! | 0x105 | read   | handle | length    | buffer |
//! | 0x106 | write  | handle | length    | buffer |
//! | 0x107 | lseek  | handle | offset    | whence |

/// `BREAK code`: the code sits in bits 6..25 of the instruction word.
macro_rules! host_call {
    ($word:literal, $a1:expr, $a2:expr, $a3:expr) => {{
        let v0: u32;
        let v1: u32;
        #[cfg(target_arch = "mips")]
        // SAFETY: a BREAK the dev kit host (or the emulator's PC link)
        // answers by setting v0/v1 and, for a read, writing the buffer the
        // caller passed; memory is not marked untouched so the compiler
        // assumes that write. Without a host it is an exception the
        // runtime's handler steps over.
        unsafe {
            core::arch::asm!(
                concat!(".word ", $word),
                in("$5") $a1 as u32,
                in("$6") $a2 as u32,
                in("$7") $a3 as u32,
                lateout("$2") v0,
                lateout("$3") v1,
                options(nostack),
            );
        }
        #[cfg(not(target_arch = "mips"))]
        {
            let _ = ($a1, $a2, $a3);
            v0 = u32::MAX;
            v1 = 0;
        }
        if v0 == 0 { v1 as i32 } else { -1 }
    }};
}

#[no_mangle]
pub extern "C" fn PCinit() -> i32 {
    host_call!("0x0000404D", 0, 0, 0)
}

#[no_mangle]
pub extern "C" fn PCcreat(name: *const u8, attrib: i32, _unused: i32) -> i32 {
    host_call!("0x0000408D", name, attrib, 0)
}

#[no_mangle]
pub extern "C" fn PCopen(name: *const u8, mode: i32, _attrib: i32) -> i32 {
    host_call!("0x000040CD", name, mode, 0)
}

#[no_mangle]
pub extern "C" fn PCclose(fd: i32) -> i32 {
    host_call!("0x0000410D", fd, 0, 0)
}

#[no_mangle]
pub extern "C" fn PCread(fd: i32, buf: *mut u8, len: i32) -> i32 {
    host_call!("0x0000414D", fd, len, buf)
}

#[no_mangle]
pub extern "C" fn PCwrite(fd: i32, buf: *const u8, len: i32) -> i32 {
    host_call!("0x0000418D", fd, len, buf)
}

#[no_mangle]
pub extern "C" fn PClseek(fd: i32, offset: i32, whence: i32) -> i32 {
    host_call!("0x000041CD", fd, offset, whence)
}

/// The dev kit's debug monitor checked here for commands from the host
/// debugger (`BREAK 0x400`). No debugger is attached: nothing to do.
#[no_mangle]
pub extern "C" fn psyq_pollhost() {}
