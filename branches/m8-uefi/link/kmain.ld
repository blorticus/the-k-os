OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
  .text.start (0x100000) : {
    obj/start.o( .text )
  }

  .text : ALIGN(0x1000) {
    *(.text)
  }

  .data : ALIGN(0x1000) {
    *(.data)
  }

  .bss : ALIGN(0x1000) {
    *(.bss)
  }
}
