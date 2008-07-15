#include <vga.h>
#include <sys/types.h>
#include <math.h>

/*
 * RE-ENTRANT?      NO 
 */

#define VGA_BASE_MEM    0xB8000


void vga_simple_putc( char c, _U16 row, _U16 col ) {
    _U16* memptr;

    memptr = (_U16 *)VGA_BASE_MEM + row * 80 + col;
    *memptr = 0x0f00 | (_U16)c;
}


_U8 vga_color;
_U8 vga_width;
_U8 vga_height;
_U8 vga_current_row;
_U8 vga_current_col;

_U16* vga_memptr;

#define VGA_ATTR ((_U16)(vga_color << 8))

void vga_init( _U8 width, _U8 height, _U8 at_row, _U8 at_col, _U8 bgcolor, _U8 fgcolor ) {
    vga_width        = width;
    vga_height       = height;
    vga_current_row  = at_row;
    vga_current_col  = at_col;
    vga_color        = (_U8)((bgcolor << 4) | fgcolor);
}


void vga_default_init( void ) {
    vga_init( 80, 25, 0, 0, black, light_gray );
}


void vga_set_location( _U8 row, _U8 column ) {
    if (row >= vga_height || column >= vga_width)
        return;

    vga_current_row = row;
    vga_current_col = column;
}


void vga_putc( char c ) {
    if (c == '\r') {
        vga_current_col = 0;
    }
    else if (c == '\n') {
        vga_current_col = 0;
        vga_current_row++;
    }
    else if (c >= ' ') {
       vga_memptr = (_U16 *)VGA_BASE_MEM + (vga_current_row * vga_width) + vga_current_col;
       *vga_memptr = c | VGA_ATTR;

        if (++vga_current_col >= vga_width) {
            vga_current_row++;
            vga_current_col = 0;
        }
    }
 
    if (vga_current_row >= vga_height) {
        vga_scroll();
        vga_current_row = vga_height - 1;
    }
}


void vga_puts( char *s ) {
    while (*s)
        vga_putc( *s++ );
}


void vga_scroll( void ) {
    _U16 i;

    _U16* dst_ptr = (_U16 *)VGA_BASE_MEM;
    _U16* src_ptr = (_U16 *)VGA_BASE_MEM + vga_width;

    /* inefficient 2byte-by-2byte copy */
    /* first, copy every row to the row above it, except the first row */
    for (i = 0; i < (vga_height - 1) * vga_width; i++)
        *dst_ptr++ = *src_ptr++;

    /* then, fill in the last row with spaces to clear any content that was there */
    for (i = 0; i < vga_width; i++)
        *dst_ptr++ = ' ' | VGA_ATTR;
}



void vga_cls( void ) {
    _U16 i;

    vga_memptr = (_U16 *)VGA_BASE_MEM;

    for (i = 0; i < vga_height * vga_width; i++)
        *vga_memptr++ = ' ' | VGA_ATTR;

    vga_current_col = 0;
    vga_current_row = 0;
}

#define M_vga_put_digit(X)                              \
    switch (X) {                                        \
        case 0: vga_putc('0'); break;                   \
        case 1: vga_putc('1'); break;                   \
        case 2: vga_putc('2'); break;                   \
        case 3: vga_putc('3'); break;                   \
        case 4: vga_putc('4'); break;                   \
        case 5: vga_putc('5'); break;                   \
        case 6: vga_putc('6'); break;                   \
        case 7: vga_putc('7'); break;                   \
        case 8: vga_putc('8'); break;                   \
        case 9: vga_putc('9');                          \
    }
 

void vga_put_dec( unsigned int n ) {
    int i, j;

    for (i = 9; i > 0; i--) {
        j = upow( 10, i );
        if (n >= j) {
            M_vga_put_digit( n / j );
            j = n % j;
        }
    }

    M_vga_put_digit( n % 10 );
}
