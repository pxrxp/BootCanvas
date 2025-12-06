/* boot.asm is assembled into binary file */
/* Everything else is first compiled into ELF */
/* The symbols (from extern, global) are linked to make a kernel.bin */
/* boot.bin and kernel.bin are concatenated later to make os.img */

OUTPUT_FORMAT(elf32-i386)
ENTRY(kstart16)
SECTIONS
{
  . = 0x1000;
  .text : {
    *kernel.o(.text)
    *(.text)
  }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : { *(.bss COMMON) }
}
