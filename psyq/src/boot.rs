//! The program's first instruction (the PS-X EXE header's initial PC, psyq.ld `__initial_pc`).
//!
//! RESTORE: PLATFORM. What the scene core's `__core_boot` will do ahead of psx-rt's `_start`
//! (scene-system CORE-ABI.md 4.3), the part a program needs before the core exists: move `$sp` to
//! the program's own stack top (`STACK_INIT`, under the core's window). A BIOS boot hands over the
//! `$sp` SYSTEM.CNF names (0x801FFFF0, inside the handoff block), not the header's. The loader's
//! argument registers `$a0..$a2` are left as they came, for `_start`.

#[cfg(target_arch = "mips")]
core::arch::global_asm!(
    r#"
    .section .text.__core_boot,"ax",@progbits
    .globl __core_boot
    .set push
    .set noreorder
__core_boot:
    lui   $sp, %hi(STACK_INIT)
    j     _start
    addiu $sp, $sp, %lo(STACK_INIT)
    .set pop
"#
);
