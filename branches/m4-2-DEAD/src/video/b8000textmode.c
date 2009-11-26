#include <video/b8000textmode.h>
#include <sys/types.h>
#include <sys/asm.h>
#include <math.h>

#define TEXTMODE_BASE_MEM    0xB8000

#ifdef TEST
    extern void TEST_b8000_write_char_at( u16 c_with_attr, void* memptr );
    #define WRITE_CHAR_AT(c,ptr) (TEST_b8000_write_char_at( c, memptr ))
#else
    #define WRITE_CHAR_AT(c,ptr) (*ptr = c)
#endif


void textmode_simple_putc( char c, u16 row, u16 col ) {
    u16* memptr;

    memptr = (u16 *)TEXTMODE_BASE_MEM + row * 80 + col;
    WRITE_CHAR_AT( (0x0f00 | (u16)c), memptr );
}


u16* textmode_memptr;

#define TEXTMODE_ATTR(vp) ((u16)(vp->color << 8))

static b8000_textmode_viewport root_vp;


b8000_textmode_viewport* retrieve_default_viewport( void ) {
    return &root_vp;
}



void textmode_configure_simple_viewport( b8000_textmode_viewport* vp, u8 start_row, u8 end_row, u8 current_row, u8 current_col ) {
    if (start_row >= end_row || current_row < start_row || current_row > end_row)
        return;

    vp->start_col   = 0;
    vp->end_col     = root_vp.end_col;
    vp->width       = root_vp.width;
    vp->start_row   = start_row;
    vp->end_row     = end_row;
    vp->height      = end_row - start_row;
    vp->current_row = current_row;;
    vp->current_col = current_col;
    vp->color       = root_vp.color;
}


void textmode_multi_scroll_vp( b8000_textmode_viewport* vp, u8 scroll_rows ) {
    if (scroll_rows == 0)
        return;

    u16 i;

    u16* dst_ptr = (u16 *)TEXTMODE_BASE_MEM + (vp->width * vp->start_row);
    u16* src_ptr = (u16 *)TEXTMODE_BASE_MEM + (vp->width * vp->start_row + vp->width * scroll_rows);

    /* inefficient 2byte-by-2byte copy */
    /* first, copy every row to the row above it, except the first row */
    for (i = vp->start_row + scroll_rows - 1; i <= vp->end_row * vp->width; i++)
        *dst_ptr++ = *src_ptr++;

    /* then, fill in the last row with spaces to clear any content that was there */
    for (i = vp->start_col; i <= vp->end_col; i++)
        *dst_ptr++ = ' ' | TEXTMODE_ATTR(vp);

// XXX: I'd like to use WRITE_CHAR_AT here, but I can't quite get it working...
//        WRITE_CHAR_AT( ((TEXTMODE_ATTR(vp)) | (u16)' '), dst_ptr );
//        dst_ptr++;
}


void textmode_scroll_vp( b8000_textmode_viewport* vp ) {
    textmode_multi_scroll_vp( vp, 1 );
}


void textmode_init( u8 width, u8 height, u8 at_row, u8 at_col, u8 bgcolor, u8 fgcolor ) {
    root_vp.start_col       = 0;
    root_vp.end_col         = width - 1;
    root_vp.width           = width;
    root_vp.start_row       = 0;
    root_vp.end_row         = height - 1;
    root_vp.height          = height;
    root_vp.current_row     = at_row;
    root_vp.current_col     = at_col;
    root_vp.color           = (u8)((bgcolor << 4) | fgcolor);

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
     * We inactivate the cursor because it's kind of annoying to track by simply setting bit 5 to 0, the
     * unused bits 6 and 7 to 0, and all the rest of the bits to 1
     */
    ioport_writeb( 0x3d4, 0x0a );
    ioport_writeb( 0x3d5, 0x1f );
}


void textmode_init_default( void ) {
    textmode_init( 80, 25, 0, 0, black, light_gray );
}

void textmode_set_location_vp( b8000_textmode_viewport* vp, u8 row, u8 column ) {
    if (row > vp->end_row || column > vp->end_col)
        return;

    vp->current_row = row;
    vp->current_col = column;
}


void textmode_set_location( u8 row, u8 column ) {
    textmode_set_location_vp( &root_vp, row, column );
}


int textmode_putchar( int c ) {
    textmode_putc( (unsigned char)c );
    return c;
}


void textmode_putc_vp( b8000_textmode_viewport* vp, char c ) {
    if (c == '\r') {
        vp->current_col = 0;
    }
    else if (c == '\n') {
        vp->current_col = 0;
        vp->current_row++;
    }
    else if (c == '\t') {
        vp->current_col += 4;
    }
    else if (c == '\b') {
        if (vp->current_col == vp->start_col) {
            if (vp->current_row > vp->start_row) {
                vp->current_row--;
                vp->current_col = vp->end_col;
                textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (vp->current_row * vp->width) + vp->current_col;
                *textmode_memptr = ' ' | TEXTMODE_ATTR(vp);
            }
        }
        else {
            vp->current_col--;
            textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (vp->current_row * vp->width) + vp->current_col;
            *textmode_memptr = ' ' | TEXTMODE_ATTR(vp);
        }
    }
    else if (c >= ' ') {
       textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (vp->current_row * vp->width) + vp->current_col;
       *textmode_memptr = c | TEXTMODE_ATTR(vp);

        if (++vp->current_col > vp->end_col) {
            vp->current_row++;
            vp->current_col = 0;
        }
    }
 
    if (vp->current_row > vp->end_row) {
        textmode_scroll_vp( vp );
        vp->current_row = vp->end_row;
    }
}


void textmode_putc( char c ) {
    textmode_putc_vp( &root_vp, c );
}



void textmode_putc_at_vp( b8000_textmode_viewport* vp, char c, u8 row, u8 col ) {
    if (row > vp->end_row || col > vp->end_col)
        return;

    u8 hold_row = vp->current_row;
    u8 hold_col = vp->current_col;

    vp->current_row = row;
    vp->current_col = col;

    textmode_putc( c );

    vp->current_row = hold_row;
    vp->current_col = hold_col;
}


void textmode_putc_at( char c, u8 row, u8 col ) {
    textmode_putc_at_vp( &root_vp, c, row, col );
}


void textmode_puts_vp( b8000_textmode_viewport* vp, char *s ) {
    while (*s)
        textmode_putc_vp( vp, *s++ );
}


void textmode_puts( char *s ) {
    textmode_puts_vp( &root_vp, s );
}


void textmode_puts_at_vp( b8000_textmode_viewport* vp, char* s, u8 row, u8 col ) {
    if (row > vp->end_row || col > vp->end_col)
        return;

    u8 hold_row = vp->current_row;
    u8 hold_col = vp->current_col;

    vp->current_row = row;
    vp->current_col = col;

    textmode_puts_vp( vp, s );

    vp->current_row = hold_row;
    vp->current_col = hold_col;
}


void textmode_puts_at( char* s, u8 row, u8 col ) {
    textmode_puts_at_vp( &root_vp, s, row, col );
}


/* static buffer for the decimal string, and a pointer to the first element in 'c' where a valid digit exists, since we'll build this backwards */
char int_buff[11];

void textmode_puti_vp( b8000_textmode_viewport* vp, unsigned int i ) {
    int_buff[10] = '\0';

    u8 last_digit = 10;

    if (i == 0)
        int_buff[--last_digit] = '0';
    else
        while (i) {
            int_buff[--last_digit] = '0' + (i % 10);
            i = i / 10;
        }

    textmode_puts_vp( vp, (char*)(int_buff + last_digit) );
}


void textmode_puti( unsigned int i ) {
    textmode_puti_vp( &root_vp, i );
}



void textmode_puti_at_vp( b8000_textmode_viewport* vp, unsigned int i, u8 row, u8 col ) {
    if (vp->current_row > vp->end_row || vp->current_col > vp->end_col)
        return;

    u8 hold_row = vp->current_row;
    u8 hold_col = vp->current_col;

    vp->current_row = row;
    vp->current_col = col;

    textmode_puti( i );

    vp->current_row = hold_row;
    vp->current_col = hold_col;
}


/* just like textmode_puti, but at a particular row and column */
void textmode_puti_at( unsigned int i, u8 row, u8 col ) {
    textmode_puti_at_vp( &root_vp, i, row, col );
}




void textmode_cls_vp( b8000_textmode_viewport* vp ) {
    u16 i;

    textmode_memptr = (u16 *)TEXTMODE_BASE_MEM;

    for (i = vp->start_row; i <= vp->end_row * vp->width; i++)
        *textmode_memptr++ = ' ' | TEXTMODE_ATTR(vp);

    vp->current_col = 0;
    vp->current_row = vp->start_row;
}


void textmode_cls( void ) {
    textmode_cls_vp( &root_vp );
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
void textmode_put_hexbyte( u8 byte ) {
    if ((byte >> 4) < 10)
        textmode_putc( (byte >> 4) + 48 );
    else
        textmode_putc( (byte >> 4) - 10 + 97 );

    if ((byte & 0x0f) < 10)
        textmode_putc( (byte & 0x0f) + 48 );
    else
        textmode_putc( (byte & 0x0f) - 10 + 97 );
}
