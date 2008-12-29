#include <video/b8000textmode.h>
#include <sys/types.h>
#include <sys/asm.h>
#include <math.h>

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

    /*
     * The VGA I/O port 0x3d4 looks first for "index" (indicating where data are to be written) and then "data", then "index", then
     * "data", and so forth.  However, we can't really tell whether the port is currently awaiting data or index.  If we write in
     * the wrong order, at best nothing happens, and at worst, terrible things.  I believe I've already seen this happen.  While we
     * can't tell what it's waiting for, we can tell it to reset, so that it next expects "index".
     * XXX: Without disabled interrupts, an interrupt handler could tromp on this.  However, in our current use, interrupts cannot be
     *      enabled, so that'll have to wait
     */
    ioport_readb( 0x3da );  // only need to read from here; don't care about the result

    /*
     * The VGA I/O port 0x0a address activates/deactivates cursor (bit 5) and the cursor start register.
     * We inactivate the cursor because it's kind of annoying to track by simply setting the entire 0x0a
     * I/O port data to 1's
     */
    ioport_writeb( 0x3d4, 0x0a );
    ioport_writeb( 0x3d5, 0x0f );
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


int textmode_putchar( int c ) {
    textmode_putc( (unsigned char)c );
    return c;
}


void textmode_putc( char c ) {
    if (c == '\r') {
        textmode_current_col = 0;
    }
    else if (c == '\n') {
        textmode_current_col = 0;
        textmode_current_row++;
    }
    else if (c == '\t') {
        textmode_current_col += 4;
    }
    else if (c == '\b') {
        if (textmode_current_col == 0) {
            if (textmode_current_row > 0) {
                textmode_current_row--;
                textmode_current_col = textmode_width - 1;
                textmode_memptr = (_U16 *)TEXTMODE_BASE_MEM + (textmode_current_row * textmode_width) + textmode_current_col;
                *textmode_memptr = ' ' | TEXTMODE_ATTR;
            }
        }
        else {
            textmode_current_col--;
            textmode_memptr = (_U16 *)TEXTMODE_BASE_MEM + (textmode_current_row * textmode_width) + textmode_current_col;
            *textmode_memptr = ' ' | TEXTMODE_ATTR;
        }
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


void textmode_putc_at( char c, _U8 row, _U8 col ) {
    if (row >= textmode_height || col >= textmode_width)
        return;

    _U8 hold_row = textmode_current_row;
    _U8 hold_col = textmode_current_col;

    textmode_current_row = row;
    textmode_current_col = col;

    textmode_putc( c );

    textmode_current_row = hold_row;
    textmode_current_col = hold_col;
}


void textmode_puts( char *s ) {
    while (*s)
        textmode_putc( *s++ );
}


void textmode_puts_at( char* s, _U8 row, _U8 col ) {
    if (row >= textmode_height || col >= textmode_width)
        return;

    _U8 hold_row = textmode_current_row;
    _U8 hold_col = textmode_current_col;

    textmode_current_row = row;
    textmode_current_col = col;

    textmode_puts( s );

    textmode_current_row = hold_row;
    textmode_current_col = hold_col;
}


/* static buffer for the decimal string, and a pointer to the first element in 'c' where a valid digit exists, since we'll build this backwards */
char int_buff[11];

void textmode_puti( unsigned int i ) {
    int_buff[10] = '\0';

    u8 last_digit = 10;

    if (i == 0)
        int_buff[--last_digit] = '0';
    else
        while (i) {
            int_buff[--last_digit] = '0' + (i % 10);
            i = i / 10;
        }

    textmode_puts( (char*)(int_buff + last_digit) );
}


/* just like textmode_puti, but at a particular row and column */
void textmode_puti_at( unsigned int i, u8 row, u8 col ) {
    if (row >= textmode_height || col >= textmode_width)
        return;

    _U8 hold_row = textmode_current_row;
    _U8 hold_col = textmode_current_col;

    textmode_current_row = row;
    textmode_current_col = col;

    textmode_puti( i );

    textmode_current_row = hold_row;
    textmode_current_col = hold_col;
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


/* Given a single unsigned byte, textmode_putc its value as two hex digits */
void textmode_put_hexbyte( _U8 byte ) {
    if ((byte >> 4) < 10)
        textmode_putc( (byte >> 4) + 48 );
    else
        textmode_putc( (byte >> 4) - 10 + 97 );

    if ((byte & 0x0f) < 10)
        textmode_putc( (byte & 0x0f) + 48 );
    else
        textmode_putc( (byte & 0x0f) - 10 + 97 );
}
