//! Entry for programs linked with boot-only memory (`psyq/psyq-boot.ld`).
//!
//! The boot-only sections are stored after `.data` in the EXE file, where
//! `.bss` will be, and run at the start of the heap. `__boot_entry` is the
//! EXE's initial PC for such programs: it moves them up (from the end
//! backwards, the two ranges may overlap), zeroes their own `.bss` part and
//! goes on to `_start`, which clears `.bss` and flushes the instruction
//! cache before any of the moved code runs. It uses no stack and leaves the
//! loader's argument registers alone. A program linked without
//! `psyq-boot.ld` never references it and the linker drops it.

#[cfg(target_arch = "mips")]
core::arch::global_asm!(
    r#"
    .section .text.__boot_entry,"ax",@progbits
    .globl __boot_entry
    .weak __boot_lma
    .weak __boot_start
    .weak __boot_load_end
    .weak __boot_end
    .set push
    .set noreorder
    .set noat
__boot_entry:
    lui   $t0, %hi(__boot_lma)
    addiu $t0, $t0, %lo(__boot_lma)
    lui   $t1, %hi(__boot_start)
    addiu $t1, $t1, %lo(__boot_start)
    lui   $t2, %hi(__boot_load_end)
    addiu $t2, $t2, %lo(__boot_load_end)
    subu  $t3, $t2, $t1
    addu  $t4, $t0, $t3
1:  beq   $t2, $t1, 2f
    nop
    addiu $t4, $t4, -4
    lw    $t5, 0($t4)
    addiu $t2, $t2, -4
    b     1b
    sw    $t5, 0($t2)
2:  lui   $t1, %hi(__boot_load_end)
    addiu $t1, $t1, %lo(__boot_load_end)
    lui   $t2, %hi(__boot_end)
    addiu $t2, $t2, %lo(__boot_end)
3:  beq   $t1, $t2, 4f
    nop
    sw    $zero, 0($t1)
    b     3b
    addiu $t1, $t1, 4
4:  j     _start
    nop
    .set pop
"#
);
