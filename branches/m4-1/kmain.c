#include <b8000textmode.h>

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


void kmain( void ) {
//    setup_gdt();
//    setup_idt();

    textmode_default_init();
    textmode_cls();

    textmode_puts( "The K-OS is now loaded.\n\n\n" );

    for (;;) ;
}
