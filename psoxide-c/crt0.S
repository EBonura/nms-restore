# Startup for C programs on PSoXide: set gp and sp, clear .bss, call main.
        .set noreorder
        .section .text.start, "ax"
        .globl _start
_start:
        la      $gp, _gp
        la      $sp, __stack_top
        la      $t0, __bss_start
        la      $t1, __bss_end
1:      sltu    $t2, $t0, $t1
        beqz    $t2, 2f
        nop
        sw      $zero, 0($t0)
        b       1b
        addiu   $t0, $t0, 4
2:      jal     main
        nop
3:      b       3b
        nop
