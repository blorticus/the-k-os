ENTRY(start)
start_of_kernel = 0x00100000;
SECTIONS
{
  .text start_of_kernel : ALIGN(4096) { *(.text) *(.rodata) }
  .data                 : ALIGN(4096) { *(.data) }
  .bss                  : ALIGN(4096) { *(.bss)  }

  end_of_kernel = .;
}
