#include <video/b8000textmode.h>
#include <video/kterm.h>
#include <sys/boot.h>
#include <sys/asm.h>
#include <idt.h>
#include <irq.h>
#include <input/keyboard.h>
#include <stdio.h>
#include <multiboot.h>

int main( void );

B8000_TEXTMODE_WINDOW w;
//B8000_TEXTMODE_WINDOW w2;
//int i;
//void _write_char_at( char c, u16* offset );

char buf[80];

void kmain( void ) {
    multiboot_relocate();

    create_gdt();

    idt_install();
    isrs_install();
    irq_install();
    keyboard_irq_install();

    textmode_init_default();
    // MUST happen after texmode initalization since isrs/irqs currently may use this mode
    enable_interrupts();

    textmode_cls();
    textmode_puts( "The K-OS is now loaded.\n\n\n" );

//  void create_textmode_window( B8000_TEXTMODE_WINDOW* w, u8 start_row, u8 window_height, u8 window_width, u8 colors ) {

//    init_textmode_window( &w, 1, 5, 80, 0x0f );
//    init_textmode_window( &w2, 7, 4, 80, 0x0f );
//    textmode_window_cls( &w );

//    textmode_window_puts( &w, "LINE 1\nLINE 2\nLINE 3\n" );
//    textmode_window_puts( &w, "LINE 1" );

//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w, 'A' );
//
//    textmode_window_cls( &w2 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w2, 'a' );
// 
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w, 'B' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w2, 'b' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w, 'C' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w2, 'c' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w, 'D' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w2, 'd' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w, 'E' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w2, 'e' );
//
//    kterm_readline( &buf, 80 );
//    textmode_window_cls( &w );
//
//    kterm_readline( &buf, 80 );
//    textmode_window_cls( &w2 );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w, 'A' );
//
//    kterm_readline( &buf, 80 );
//    for (i = 0; i < 80; i++)
//        textmode_window_putc( &w2, '1' );

    main();  /* call into KoSH */

    textmode_puts( "System Halted." );

    for (;;) ;
}
