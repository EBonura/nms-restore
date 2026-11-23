/*
 * Link a Psy-Q layer program: NMS's C objects plus libpsyq.a (psx-rt's
 * `_start` and runtime, the layer, libc, libm, the compiler's soft-float).
 *
 * Adapted from the PSoXide SDK's sdk/psoxide.ld (same PS-X EXE header,
 * filled from link-time symbols; the image loads at 80010000h) for ld.lld, the SDK's linker, and GNU ld
 * (the header's end is `__exe_head_start + EXE_HEAD_BYTES`, as `.` inside a section is an absolute
 * address to lld and an offset to GNU ld; the output format is the linker's default),
 * with the RAM size a parameter: --defsym=RAM_SIZE=0x800000 for the dev
 * kit's 8 MB, which NMS's 1996 builds were linked for, or the default for a
 * console: the RAM below the SDK scene core's window (RESTORE: PLATFORM; the
 * build scripts pass psyq/core-window.sh's value, 0x1FE000 today, which this
 * default repeats: window 0x801FDF00..0x801FFF00, its handoff block above it
 * to 0x80200000). The stack starts 100h below the top, at 0x801FDF00 today,
 * where `__core_boot` (src/boot.rs) puts it first thing.
 *
 * A program with scene images links -T its images' script (tools/scenes.py)
 * -T psyq.ld: the images share one region after .data, and are not in the EXE.
 *
 * NMS's code is compiled with -G0, so nothing is addressed from $gp.
 * `objcopy -O binary` of the header, .text and .data is the EXE; qdup/build.sh
 * pads it to whole 800h-byte sectors.
 */
ENTRY(_start)
__initial_pc = __core_boot;

RAM_BASE       = 0x80000000;
RAM_SIZE       = DEFINED(RAM_SIZE) ? RAM_SIZE : 0x1FE000;
LOAD_ADDR      = 0x80010000;
EXE_HEAD_BYTES = 0x800;
STACK_INIT     = RAM_BASE + RAM_SIZE - 0x100;
/* --defsym=STACK_RESERVE_OVERRIDE=N keeps less than 32 KB for the stack (360X keeps
 * 16 KB: its Mission 1 needs the room; its stack was measured at 1.3 KB). The SDK script's hook. */
STACK_RESERVE  = DEFINED(STACK_RESERVE_OVERRIDE) ? STACK_RESERVE_OVERRIDE : 0x8000;

SECTIONS {
    . = LOAD_ADDR - EXE_HEAD_BYTES;
    .ps_exe_head : {
        __exe_head_start = .;
        LONG(0x582D5350);                   /* "PS-X" */
        LONG(0x45584520);                   /* " EXE" */
        LONG(0);
        LONG(0);
        LONG(DEFINED(__initial_pc) ? __initial_pc : _start);   /* initial PC: __core_boot, then _start */
        LONG(0);                            /* initial GP: unused */
        LONG(LOAD_ADDR);                    /* destination */
        LONG(__image_end - __text_start);   /* bytes after the header */
        LONG(0);
        LONG(0);
        LONG(0);                            /* BSS: `_start` clears it */
        LONG(0);
        LONG(STACK_INIT);                   /* SP base */
        LONG(0);
        . = __exe_head_start + EXE_HEAD_BYTES;
    }

    .text LOAD_ADDR : {
        __text_start = .;
        KEEP(*(.text._start))
        *(.text.hot .text.hot.*)            /* kept together for the I-cache */
        /* The direct-access guard's window (sdk/docs/PRESENT.md, "A program with its own linker
           script"): where psx-rt's `wait_idle` goes, so the symbol below says whether it is linked. */
        . = ALIGN(4);
        __psx_guard_window_start = .;
        *(.text.__psx_rt_wait_idle)
        __psx_guard_window_end = .;
        /* I-cache placement (360X: the layer's code moved about 3 KB against the game's hot C code and the
           I-cache stalls doubled). The game's own objects first, then the layer's few hot functions
           (`#[link_section = ".text.layer_hot"]` in gs.rs), then the rest of the layer: the hot
           functions sit at one fixed distance from the C code, whatever the layer around them
           grows or shrinks to. Derived from the I-cache stall log of the 360X route (see
           docs/DECISIONS.md, "I-cache placement"). */
        *(EXCLUDE_FILE(*libpsyq.a:*) .text .text.*)
        . = ALIGN(16);
        *(.text.layer_warm)
        /* the hot functions start on I-cache line 252 (address & 0xfff = 0xfc0), clear of the C code's
           hot lines (measured: lines 35 to 214 are the sprite and map code, 129 to 130 the VSync wait) */
        . = ALIGN(16);
        . += (0xfc0 - (. & 0xfff)) & 0xfff;
        *(.text.layer_hot)
        *(.text .text.*)
        /* Cold code after everything else (the SDK's psoxide.ld does the same): the VBlank handler's
           draw-done confirmation, `.psx_cold.psx_rt_flip_confirm`, which a program that presents links.
           Left out of the text, it would be an orphan section the executable does not carry. */
        *(.psx_cold .psx_cold.*)
        __text_end = .;
    }
    __psx_direct_access_guard = __psx_guard_window_end > __psx_guard_window_start ? __psx_guard_window_start : 0;

    .data : {
        __data_start = .;
        *(.data .data.*)
        *(.rodata .rodata.*)
        *(.sdata .sdata.*)
        *(.got)
        . = ALIGN(4);
        __data_end = .;
    }
    __load_end  = __data_end;
    __image_end = __text_start + ((__load_end - __text_start + EXE_HEAD_BYTES - 1) & ~(EXE_HEAD_BYTES - 1));

    .bss (NOLOAD) : {
        __bss_start = .;
        *(.sbss .sbss.* .scommon)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    }

    __heap_start = __bss_end;
    /* a program without scene images (tools/scenes.py) has an empty region */
    PROVIDE(__scene_region_start = 0);
    PROVIDE(__scene_region_end = 0);
    __heap_end   = STACK_INIT - STACK_RESERVE;
    ASSERT(__bss_end <= STACK_INIT - STACK_RESERVE, "program does not fit below the stack reserve")

    /DISCARD/ : {
        *(.region)
        *(.MIPS.abiflags)
        *(.reginfo)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.comment)
        *(.note.*)
        *(.pdr)
        *(.gnu.attributes)
        *(.mdebug.*)
    }
}
