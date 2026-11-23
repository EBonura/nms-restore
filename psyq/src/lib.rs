//! A clean-room replacement for the parts of Sony's Psy-Q libraries that NMS
//! Software's 1996 games call, on the PSoXide SDK.
//!
//! The games stay in C as NMS wrote them; this crate is the library they link
//! against instead of Sony's. Every function here has the C name and calling
//! convention the game expects and is declared in a C header under
//! `psyq/include`. Each was written from how NMS's code uses it, from the
//! hardware's documented behaviour, and where it mattered from what NMS's
//! own 1996 build observably did (see `psyq/README.md`), never from Sony's
//! code, headers or documentation.
//!
//! The program image is psx-rt's: `__core_boot` (boot.rs) moves the stack under the scene core's
//! window, psx-rt's `_start` clears `.bss`, sets the machine to a known state and calls [`main`],
//! which hands over to the game's `main` (compiled as `nms_main`).
//!
//! What is not implemented is a stub that says so once on the debug console
//! (`psyq: STUB name`), so a run shows which ones the game reached.
#![no_std]
#![feature(c_variadic)]
#![cfg_attr(target_arch = "mips", feature(asm_experimental_arch))]
#![allow(non_snake_case, clippy::missing_safety_doc)]

extern crate psx_rt;
// RESTORE: PLATFORM. QD Up's renderer (escher/): its E_* functions are exported from this one static library.
extern crate escher;

#[macro_use]
mod console;
mod card;
mod cd;
mod core_shim;
mod path;
mod boot;
mod fmv;
mod font;
mod gpu;
mod gs;
mod gte;
mod kernel;
mod libc;
mod libm;
mod loadfail;
mod pcdrv;
mod pmd;
mod scenes;
mod snd;
mod splash;
mod stubs;

extern "C" {
    /// The game's `main`, renamed at compile time (`-Dmain=nms_main`).
    fn nms_main();
}

/// psx-rt's `_start` calls this once the machine is set up.
#[no_mangle]
pub fn main() {
    console::puts("psyq: PSoXide Psy-Q layer v0\n");
    // RESTORE: PLATFORM. The handoff block the scene core will write before `_start` (core_shim.rs).
    core_shim::attach();
    // SAFETY: the game's entry point, called once on the one thread there is.
    unsafe { nms_main() };
    console::puts("psyq: the game's main returned\n");
}
