#include <vga.h>

// 
// 
// /**
//  * Configures a Global Descriptor Table (GDT) with four entries:
//  *   0x00 - null
//  *   0x08 - code
//  *   0x10 - data
//  *   0x18 - idt
//  *   0x20 - tss
//  * Moves CS to 0x08, ds/ss/es/fs/gs to 0x10
//  **
// void setup_gdt( void ) {
// 
// }
// 
// 
// /**
//  * Configures an Interrupt Descriptor Table (IDT) with entries for int0h - int10h
//  * Each of them does nothing for now
//  */
// void setup_idt( void ) {
// 
// }
// 
// 
// #define VGA_BASE_MEM    0xB8000
// 
// 
// void vga_simple_putc( char c, _U16 row, _U16 col ) {
// //    _U16* memptr;
// //
// //    memptr = (_U16 *)VGA_BASE_MEM + row * 80 + col;
// //    *memptr = 0x0f00 | (_U16)c;
// }



void kmain( void ) {
    int i;
//    setup_gdt();
//    setup_idt();

    vga_default_init();
    vga_cls();

    vga_puts( "The K-OS is now loaded.\n\n\n" );

    for (;;) ;
}
