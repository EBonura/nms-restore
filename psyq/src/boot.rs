//! Where the scene core's `__core_boot` (psx-core, the header's initial PC) goes next (psyq.ld `__core_next`).
//!
//! RESTORE: PLATFORM. What the scene core's `__core_boot` will do ahead of psx-rt's `_start`
//! (scene-system CORE-ABI.md 4.3), the part a program needs before the core exists: move `$sp` to
//! the program's own stack top (`STACK_INIT`, under the core's window). A BIOS boot hands over the
//! `$sp` SYSTEM.CNF names (0x801FFFF0, inside the handoff block), not the header's. The loader's
//! argument registers `$a0..$a2` are left as they came, for `_start`.
//!
//! First, a program whose EXE carries a boot scene image (tools/scenes.py `boot`: the splash and the film
//! player) has it moved from where the EXE stored it (after the preloaded front end, over what will be
//! `.bss`) up to the start of the heap, where it runs, from the end backwards; `_start` then clears `.bss`
//! and sets the machine's entry state, which flushes the instruction cache, before any of it runs. The
//! symbols are psyq.ld's zeros for a program without one.

#[cfg(target_arch = "mips")]
core::arch::global_asm!(
    r#"
    .section .text.__psyq_boot_entry,"ax",@progbits
    .globl __psyq_boot_entry
    .set push
    .set noreorder
__psyq_boot_entry:
    lui   $t0, %hi(__scene_boot_lma)
    addiu $t0, $t0, %lo(__scene_boot_lma)
    lui   $t1, %hi(__scene_boot_vma)
    addiu $t1, $t1, %lo(__scene_boot_vma)
    lui   $t2, %hi(__scene_boot_size)
    addiu $t2, $t2, %lo(__scene_boot_size)
    beq   $t2, $zero, 2f
    addu  $t0, $t0, $t2
    addu  $t1, $t1, $t2
1:  addiu $t0, $t0, -4
    lw    $t3, 0($t0)
    addiu $t1, $t1, -4
    addiu $t2, $t2, -4
    bnez  $t2, 1b
    sw    $t3, 0($t1)
2:  j     _start
    nop
    .set pop
"#
);
