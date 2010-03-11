#include <video/b8000textmode.h>
#include <sys/types.h>
#include <sys/asm.h>
#include <math.h>

//#define TEXTMODE_BASE_MEM    0xB8000

//#ifdef TEST
//    extern void TEST_b8000_write_char_at( u16 c_with_attr, void* memptr );
//    #define WRITE_CHAR_AT(c,ptr) (TEST_b8000_write_char_at( c, memptr ))
//#else
//    #define WRITE_CHAR_AT(c,ptr) (*(ptr) = (c))
//#endif


void textmode_simple_putc( char c, _U16 row, _U16 col ) {
    _U16* memptr;

    memptr = (_U16 *)TEXTMODE_BASE_MEM + row * 80 + col;
    *memptr = 0x0f00 | (_U16)c;
}


/* start_row starts at 1 */
void init_textmode_window( B8000_TEXTMODE_WINDOW* w, u8 start_row, u8 window_height, u8 window_width, u8 colors ) {
    w->width = window_width;
    w->attrs = ((u16)(colors << 8));
    w->first_pos = (u16*)TEXTMODE_BASE_MEM + ((start_row - 1) * window_width);
    w->last_pos  = w->first_pos + (window_width * window_height);
    w->pos = w->first_pos;
}


/*
 * Starting at first_pos (offset from start of textmode memory) copy characters through last_pos back copy_back
 * positions.  If copy_back > first_pos , or first_pos > last_pos, silently return.
 */
void textmode_copy_back( u16 first_pos, u16 last_pos, u16 copy_back ) {
    if (copy_back > first_pos)
        return;

    u16* dest = (u16*)((u16*)(TEXTMODE_BASE_MEM) + first_pos - copy_back);
    u16* src  = (u16*)((u16*)(TEXTMODE_BASE_MEM) + first_pos);

    while (first_pos++ <= last_pos)
        *dest++ = *src++;
}


void textmode_window_scroll( B8000_TEXTMODE_WINDOW* w ) {
    u16* d = w->first_pos;
    u16* s = w->first_pos + w->width;
    while (s <= w->last_pos)
        *d++ = *s++;

    /* clear last line */
    while (d <= w->last_pos)
        *d++ = ' ' | w->attrs;

    w->pos = w->last_pos - w->width + 1;
}


void textmode_window_putc( B8000_TEXTMODE_WINDOW* w, unsigned char c ) {
    int i;

    if (c == '\r') {
        w->pos -= (w->pos - w->first_pos) % w->width;
    }
    else if (c == '\n') {
        w->pos += (w->width - ((w->pos - w->first_pos) % w->width));
    }
    else if (c == '\t') {
        for (i = 0; i < 4; i++)
            textmode_window_putc( w, ' ' );
    }
    else if (c == '\b') {
        if (w->pos > w->first_pos) {
            w->pos--;
//            WRITE_CHAR_AT( (w->attrs | (u16)' '), w->pos );
//            M_B8000_write_char_at( w->pos, w->attrs, ' ' );
        }
    }
    else if (c >= ' ') {
//        WRITE_CHAR_AT( (w->attrs | (u16)c), w->pos );
//        M_B8000_write_char_at( w->pos, w->attrs, c );
        if (++w->pos > w->last_pos)
            textmode_window_scroll( w );
    }
}


void textmode_window_set_pos( B8000_TEXTMODE_WINDOW* w, u8 row, u8 col ) {
    w->pos = w->first_pos + (row * w->width) + col;
}


void textmode_window_cls( B8000_TEXTMODE_WINDOW* w ) {
    w->pos = w->first_pos;
    while (w->pos <= w->last_pos)
        *(w->pos++) = ' ' | w->attrs;

    w->pos = w->first_pos;
}


void textmode_window_puts( B8000_TEXTMODE_WINDOW* w, char* s ) {
       while (*s)
        textmode_window_putc( w, *s++ );
}


int textmode_window_putchar( B8000_TEXTMODE_WINDOW* w, int c ) {
    textmode_window_putc( w, (unsigned char)c );
    return c;
}


/* static buffer for the decimal string, and a pointer to the first element in 'c' where a valid digit exists, since we'll build this backwards */
char int_buff[11];

void textmode_window_puti( B8000_TEXTMODE_WINDOW* w, unsigned int i ) {
    int_buff[10] = '\0';

    u8 last_digit = 10;

    if (i == 0)
        int_buff[--last_digit] = '0';
    else
        while (i) {
            int_buff[--last_digit] = '0' + (i % 10);
            i = i / 10;
        }

    textmode_window_puts( w, (char*)(int_buff + last_digit) );
}


#define M_textmode_put_digit(w,X)                       \
    switch (X) {                                        \
        case 0: textmode_window_putc(w,'0'); break;     \
        case 1: textmode_window_putc(w,'1'); break;     \
        case 2: textmode_window_putc(w,'2'); break;     \
        case 3: textmode_window_putc(w,'3'); break;     \
        case 4: textmode_window_putc(w,'4'); break;     \
        case 5: textmode_window_putc(w,'5'); break;     \
        case 6: textmode_window_putc(w,'6'); break;     \
        case 7: textmode_window_putc(w,'7'); break;     \
        case 8: textmode_window_putc(w,'8'); break;     \
        case 9: textmode_window_putc(w,'9');            \
    }
 

void textmode_window_put_dec( B8000_TEXTMODE_WINDOW* w, unsigned int n ) {
    int i, j;

    for (i = 9; i > 0; i--) {
        j = upow( 10, i );
        if (n >= j) {
            M_textmode_put_digit( w, n / j );
            j = n % j;
        }
    }

    M_textmode_put_digit( w, n % 10 );
}


/* Given a single unsigned byte, textmode_putc its value as two hex digits */
void textmode_window_put_hexbyte( B8000_TEXTMODE_WINDOW* w, u8 byte ) {
    if ((byte >> 4) < 10)
        textmode_window_putc( w, (byte >> 4) + 48 );
    else
        textmode_window_putc( w, (byte >> 4) - 10 + 97 );

    if ((byte & 0x0f) < 10)
        textmode_window_putc( w, (byte & 0x0f) + 48 );
    else
        textmode_window_putc( w, (byte & 0x0f) - 10 + 97 );
}


B8000_TEXTMODE_WINDOW default_window;

void textmode_init( u8 width, u8 height, u8 at_row, u8 at_col, u8 bgcolor, u8 fgcolor ) {
    init_textmode_window( &default_window, 1, height, width, (u8)((bgcolor << 4) | fgcolor) );

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


void textmode_init_default( void ) {
    textmode_init( 80, 25, 0, 0, black, light_gray );
}


void textmode_set_location( u8 row, u8 column ) {
    textmode_window_set_pos( &default_window, row, column );
}


int textmode_putchar( int c ) {
    textmode_window_putc( &default_window, (unsigned char)c );
    return c;
}


void textmode_putc( char c ) {
    textmode_window_putc( &default_window, c );
}


void textmode_puts( char *s ) {
    while (*s)
        textmode_window_putc( &default_window, *s++ );
}


void textmode_puti( unsigned int i ) {
    textmode_window_puti( &default_window, i );
}


void textmode_scroll( void ) {
    textmode_window_scroll( &default_window );
}



void textmode_cls( void ) {
    textmode_window_cls( &default_window );
}


void textmode_put_dec( unsigned int n ) {
    textmode_window_put_dec( &default_window, n );
}


void textmode_put_hexbyte( u8 byte ) {
    textmode_window_put_hexbyte( &default_window, byte );
}
