#include <b8000textmode.h>
#include <sys/types.h>
#include <math.h>

/*
 * RE-ENTRANT?      NO 
 */

#define TEXTMODE_BASE_MEM    0xB8000


void textmode_simple_putc( char c, _U16 row, _U16 col ) {
    _U16* memptr;

    memptr = (_U16 *)TEXTMODE_BASE_MEM + row * 80 + col;
    *memptr = 0x0f00 | (_U16)c;
}


_U8 textmode_color;
_U8 textmode_width;
_U8 textmode_height;
_U8 textmode_current_row;
_U8 textmode_current_col;

_U16* textmode_memptr;

#define TEXTMODE_ATTR ((_U16)(textmode_color << 8))

void textmode_init( _U8 width, _U8 height, _U8 at_row, _U8 at_col, _U8 bgcolor, _U8 fgcolor ) {
    textmode_width        = width;
    textmode_height       = height;
    textmode_current_row  = at_row;
    textmode_current_col  = at_col;
    textmode_color        = (_U8)((bgcolor << 4) | fgcolor);
}


void textmode_default_init( void ) {
    textmode_init( 80, 25, 0, 0, black, light_gray );
}


void textmode_set_location( _U8 row, _U8 column ) {
    if (row >= textmode_height || column >= textmode_width)
        return;

    textmode_current_row = row;
    textmode_current_col = column;
}


void textmode_putc( char c ) {
    if (c == '\r') {
        textmode_current_col = 0;
    }
    else if (c == '\n') {
        textmode_current_col = 0;
        textmode_current_row++;
    }
    else if (c >= ' ') {
       textmode_memptr = (_U16 *)TEXTMODE_BASE_MEM + (textmode_current_row * textmode_width) + textmode_current_col;
       *textmode_memptr = c | TEXTMODE_ATTR;

        if (++textmode_current_col >= textmode_width) {
            textmode_current_row++;
            textmode_current_col = 0;
        }
    }
 
    if (textmode_current_row >= textmode_height) {
        textmode_scroll();
        textmode_current_row = textmode_height - 1;
    }
}


void textmode_puts( char *s ) {
    while (*s)
        textmode_putc( *s++ );
}


void textmode_scroll( void ) {
    _U16 i;

    _U16* dst_ptr = (_U16 *)TEXTMODE_BASE_MEM;
    _U16* src_ptr = (_U16 *)TEXTMODE_BASE_MEM + textmode_width;

    /* inefficient 2byte-by-2byte copy */
    /* first, copy every row to the row above it, except the first row */
    for (i = 0; i < (textmode_height - 1) * textmode_width; i++)
        *dst_ptr++ = *src_ptr++;

    /* then, fill in the last row with spaces to clear any content that was there */
    for (i = 0; i < textmode_width; i++)
        *dst_ptr++ = ' ' | TEXTMODE_ATTR;
}



void textmode_cls( void ) {
    _U16 i;

    textmode_memptr = (_U16 *)TEXTMODE_BASE_MEM;

    for (i = 0; i < textmode_height * textmode_width; i++)
        *textmode_memptr++ = ' ' | TEXTMODE_ATTR;

    textmode_current_col = 0;
    textmode_current_row = 0;
}

#define M_textmode_put_digit(X)                              \
    switch (X) {                                             \
        case 0: textmode_putc('0'); break;                   \
        case 1: textmode_putc('1'); break;                   \
        case 2: textmode_putc('2'); break;                   \
        case 3: textmode_putc('3'); break;                   \
        case 4: textmode_putc('4'); break;                   \
        case 5: textmode_putc('5'); break;                   \
        case 6: textmode_putc('6'); break;                   \
        case 7: textmode_putc('7'); break;                   \
        case 8: textmode_putc('8'); break;                   \
        case 9: textmode_putc('9');                          \
    }
 

void textmode_put_dec( unsigned int n ) {
    int i, j;

    for (i = 9; i > 0; i--) {
        j = upow( 10, i );
        if (n >= j) {
            M_textmode_put_digit( n / j );
            j = n % j;
        }
    }

    M_textmode_put_digit( n % 10 );
}
