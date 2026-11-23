/*
 * Boot-only memory, opt-in: link with `-T psyq-boot.ld -T psyq.ld` (this
 * script first) and --defsym=BOOT_VMA=..., --defsym=BOOT_LMA=..., -u __boot_entry.
 *
 * The sections named here are code and data a program needs only once, at
 * boot, before its heap fills: the film player (the SDK's psx-fmv and the
 * layer's FmvPlay and FmvMemoryBytes, psyq/src/fmv.rs) and the boot splash
 * (PsxSplash, psyq/src/splash.rs, drawn with SDK pieces). They are placed at
 * the start of the heap (BOOT_VMA = __bss_end), so the program's heap block
 * lies over them and takes their memory once it grows that far. In the EXE
 * file they follow .data (BOOT_LMA = __data_end), where .bss will be;
 * __boot_entry (psyq/src/boot.rs) moves them up before `_start` clears .bss.
 *
 * The program must be done with them before its heap use reaches them.
 * 360X plays its film once, first thing, with the heap all but empty, and
 * its allocator fills the block from the top down (JALLOC.C).
 *
 * The two addresses come from a first link with placeholder values (the
 * sections here keep their sizes, so .data and .bss end where they did):
 * 360x/build.sh does both links and checks the result.
 */
/* The initial PC (psyq.ld reads `__initial_pc`, so a program without this script never references
   `__boot_entry`: ld.lld keeps what a script expression names, taken or not). */
__initial_pc = __boot_entry;
SECTIONS {
    .boot BOOT_VMA : AT(BOOT_LMA) {
        __boot_start = .;
        *(.boot_only .boot_only.*)
        *(.text.FmvPlay .text.FmvMemoryBytes .text.*psx_fmv*)
        *(.rodata.*psx_fmv*)
        *(.data.*psx_fmv*)
        /* the boot splash (psyq/src/splash.rs): PsxSplash (its screen, mark, logo and lines are
           `.boot_only.*` above) and the layer's pad read for it; the rest of what it calls is shared
           with the layer's other code */
        *(.text.PsxSplash .text.*pad1_buttons*)
        /* and these SDK functions, which only the splash calls (check-boot.sh proves it) */
        *(.text.*FontAtlas10text_width* .text.*FontAtlas18draw_text_gradient* .text.*DoubleBuffer5clear*
          .text.*DoubleBuffer4swap* .text.*QuadTexturedMaterial13with_material* .text.*try_wait_vblank*
          .text.*prim19QuadTexturedGouraud*)
        /* BOOT_EXTRA */
        . = ALIGN(4);
        __boot_load_end = .;
    }
    .boot_bss (NOLOAD) : {
        *(.bss.*psx_fmv*)
        . = ALIGN(4);
        __boot_end = .;
    }
    __boot_lma = BOOT_LMA;
}
/* ld.lld takes a script with no INSERT for the main one; this is the addition (GNU ld takes it too).
   INSERT must directly follow SECTIONS's closing brace. */
INSERT AFTER .bss;
