#include <vga.h>

void kmain( void ) {
    vga_simple_putc( 'k', 5, 0 );
    vga_simple_putc( '-', 5, 1 );
    vga_simple_putc( 'o', 5, 2 );
    vga_simple_putc( 's', 5, 3 );

    vga_init();
    vga_set_location( 8, 0 );

    vga_putc( 'V' );
    vga_putc( 'E' );
    vga_putc( 'R' );

    vga_set_location( 9, 0 );
    vga_puts( "When in Rome, Rock with K-OS" );


    for (;;) ;
}

