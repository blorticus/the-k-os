#include "testlib.h"

#define M_vga_put_digit(X)                              \
    switch (X) {                                        \
        case 0: putchar('0'); break;                   \
        case 1: putchar('1'); break;                   \
        case 2: putchar('2'); break;                   \
        case 3: putchar('3'); break;                   \
        case 4: putchar('4'); break;                   \
        case 5: putchar('5'); break;                   \
        case 6: putchar('6'); break;                   \
        case 7: putchar('7'); break;                   \
        case 8: putchar('8'); break;                   \
        case 9: putchar('9');                          \
    }
 

unsigned int upow( unsigned int base, unsigned short exp ) {
    unsigned int result = base;

    while (--exp)
        result *= base;

    return result;
}



void vga_put_dec( unsigned int n ) {
    int i, j;

    for (i = 9; i > 0; i--) {
        j = upow( 10, i );
        if (n >= j)
            M_vga_put_digit( n / j );
    }

    M_vga_put_digit( n % 10 );
}
