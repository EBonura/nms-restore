/* Link C programs for a retail PS1: 2 MiB RAM, code from 0x80010000. */
OUTPUT_FORMAT("elf32-littlemips")
ENTRY(_start)
MEMORY { RAM (rwx) : ORIGIN = 0x80010000, LENGTH = 0x1F0000 }
__stack_top = 0x801FFF00;
SECTIONS {
  .text : { KEEP(*(.text.start)) *(.text .text.*) } > RAM
  .rodata : { *(.rodata .rodata.*) } > RAM
  .data : { *(.data .data.*) } > RAM
  _gp = ALIGN(16) + 0x7FF0;
  .sdata : { *(.sdata .sdata.*) } > RAM
  . = ALIGN(4);
  __load_end = .;
  __bss_start = .;
  .sbss (NOLOAD) : { *(.sbss .sbss.* .scommon) } > RAM
  .bss (NOLOAD) : { *(.bss .bss.* COMMON) } > RAM
  . = ALIGN(4);
  __bss_end = .;
  /DISCARD/ : { *(.MIPS.abiflags) *(.reginfo) *(.comment) *(.pdr) *(.gnu.attributes) }
}
