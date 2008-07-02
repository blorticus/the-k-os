#include <vga.h>

void kmain( void ) {
    vga_init();

    vga_set_location( 6, 0 );

    vga_putc( 'k' );
    vga_putc( '-' );
    vga_putc( 'o' );
    vga_putc( 's' );

    char* s = "When in Rome, Rock as K-OS Does";

    vga_set_location( 7, 0 );
    vga_puts( s );

    for (;;) ;
}

