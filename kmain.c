//#include <sys/types.h>
#include <vga.h>

//void silly_kos( void ) __attribute__((cdecl));

void kmain( void ) {
//    unsigned short *memptr;
//    memptr = (unsigned short *)0xB8320;
//    *memptr = 0x0f00 | (unsigned short)'k';
//    *memptr++ = 'O' | 0x1b;
//    *memptr   = 'S' | 0x1b;
//    vga_simple_putc( 'K', 7, 0 );
//    vga_simple_putc( 'O', 7, 1 );
//    vga_simple_putc( 'S', 7, 2 );

//    silly_kos();

    vga_simple_putc( 'k', 5, 0 );
    vga_simple_putc( '-', 5, 1 );
    vga_simple_putc( 'o', 5, 2 );
    vga_simple_putc( 's', 5, 3 );

    for (;;) ;
}

