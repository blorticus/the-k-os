ENTRY(start)
start_of_kernel = 0x00100000;
SECTIONS
{
  .text start_of_kernel : AT(start_of_kernel) {
    code = .;
    *(.text)
    *(.rodata)
    . = ALIGN(4096);
  }
  .data : AT(start_of_kernel + (data - code))
  {
    data = .;
    *(.data)
    . = ALIGN(4096);
  }
  .bss : AT(start_of_kernel + (bss - code))
  {
    bss = .;
    *(.bss)
    . = ALIGN(4096);
  }
  end_of_kernel = .;
}
