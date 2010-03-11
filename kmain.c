#include <video/kterm.h>
#include <sys/boot.h>
#include <sys/asm.h>
#include <idt.h>
#include <irq.h>
#include <input/keyboard.h>
#include <stdio.h>
#include <multiboot.h>

int main( void );

//char buf[80];
int row, col;

kterm_window rw;
KTERM_WINDOW kterm = &rw;

void kmain( void ) {
    multiboot_relocate();

    create_gdt();

    idt_install();
    isrs_install();
    irq_install();
    keyboard_irq_install();

    // MUST happen after keyboard initalization since isrs/irqs currently may use this mode
    enable_interrupts();

    kterm_init( 25, 80 );
    kterm_dup_root_window( kterm );

    kterm_window_cls( kterm );
//    kterm_window_printf( kterm, "height = %d, width = %d, base = %d, end = %d\n", kterm->width, kterm->height, kterm->base_offset, kterm->end_offset );
    kterm_window_puts( kterm, "The K-OS is now loaded.\n\n\n" );

//    for (row = 0; row < 26; row++)
//        for (col = 0; col < 80; col++)
//            kterm_window_putc( kterm, 'A' + col % 26 );

//    for (col = 0; col < 80; col++)
//        kterm_window_putc( 'Z' );

//    textmode_cls();
//    textmode_puts( "The K-OS is now loaded.\n\n\n" );
//
    main();  /* call into KoSH */

    kterm_window_cls( kterm );
    kterm_window_puts( kterm, "System Halted.\n" );

    for (;;) ;
}
