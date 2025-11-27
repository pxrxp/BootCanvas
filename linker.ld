/* boot.asm is assembled into binary file */
/* Everything else is first compiled into ELF */
/* The symbols (from extern, global) are linked to make a kernel.bin */
/* boot.bin and kernel.bin are concatenated later to make os.img */

OUTPUT_FORMAT(elf32-i386)
ENTRY(kstart16)
SECTIONS
{
  . = 0x1000;
  .text ALIGN(4K) :
  {
    *(kernel.o)
    *(.text)
  }
  .data ALIGN(4K) :
  {
    *(.data)
  }
  .bss ALIGN(4K) :
  {
    *(.bss)
  }
}
