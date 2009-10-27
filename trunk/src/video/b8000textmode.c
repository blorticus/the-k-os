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
//    *memptr = 0x0f00 | (u16)c;
    WRITE_CHAR_AT( (0x0f00 | (u16)c), memptr );
}


//u8 textmode_color;
//u8 textmode_width;
//u8 textmode_height;
//u8 textmode_current_row;
//u8 textmode_current_col;

u16* textmode_memptr;

#define TEXTMODE_ATTR(vp) ((u16)(vp->color << 8))

static b8000_textmode_viewport root_vp;


static void textmode_scroll_vp( b8000_textmode_viewport* vp ) {
    u16 i;

    u16* dst_ptr = (u16 *)TEXTMODE_BASE_MEM;
    u16* src_ptr = (u16 *)TEXTMODE_BASE_MEM + vp->width;

    /* inefficient 2byte-by-2byte copy */
    /* first, copy every row to the row above it, except the first row */
//    for (i = 0; i < (textmode_height - 1) * textmode_width; i++)
    for (i = vp->start_row; i <= vp->end_row * vp->width; i++)
        *dst_ptr++ = *src_ptr++;

    /* then, fill in the last row with spaces to clear any content that was there */
//    for (i = 0; i < textmode_width; i++)
    for (i = vp->start_col; i <= vp->end_col; i++)
        *dst_ptr++ = ' ' | TEXTMODE_ATTR(vp);

// XXX: I'd like to use WRITE_CHAR_AT here, but I can't quite get it working...
//        WRITE_CHAR_AT( ((TEXTMODE_ATTR(vp)) | (u16)' '), dst_ptr );
//        dst_ptr++;
}


//static void textmode_scroll( void ) {
//    textmode_scroll_vp( &root_vp );
//}

void textmode_init( u8 width, u8 height, u8 at_row, u8 at_col, u8 bgcolor, u8 fgcolor ) {
//    textmode_width        = width;
//    textmode_height       = height;
//    textmode_current_row  = at_row;
//    textmode_current_col  = at_col;
//    textmode_color        = (u8)((bgcolor << 4) | fgcolor);

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
     * We inactivate the cursor because it's kind of annoying to track by simply setting the entire 0x0a
     * I/O port data to 1's
     * XXX: THIS DOESN'T WORK AND NEEDS MORE RESEARCH
     */
    ioport_writeb( 0x3d4, 0x0a );
    ioport_writeb( 0x3d5, 0x0f );
}


void textmode_init_default( void ) {
    textmode_init( 80, 25, 0, 0, black, light_gray );
}

void textmode_set_location_vp( b8000_textmode_viewport* vp, u8 row, u8 column ) {
//    if (row >= textmode_height || column >= textmode_width)
    if (row > vp->end_row || column > vp->end_col)
        return;

//    textmode_current_row = row;
    vp->current_row = row;
//    textmode_current_col = column;
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
//        textmode_current_col = 0;
        vp->current_col = 0;
    }
    else if (c == '\n') {
//        textmode_current_col = 0;
//        textmode_current_row++;
        vp->current_col = 0;
        vp->current_row++;
    }
    else if (c == '\t') {
//        textmode_current_col += 4;
        vp->current_col += 4;
    }
    else if (c == '\b') {
//        if (textmode_current_col == 0) {
        if (vp->current_col == vp->start_col) {
//            if (textmode_current_row > vp.start_row) {
            if (vp->current_row > vp->start_row) {
//                textmode_current_row--;
                vp->current_row--;
//                textmode_current_col = textmode_width - 1;
                vp->current_col = vp->end_col;
//                textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (textmode_current_row * textmode_width) + textmode_current_col;
                textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (vp->current_row * vp->width) + vp->current_col;
                *textmode_memptr = ' ' | TEXTMODE_ATTR(vp);
            }
        }
        else {
//            textmode_current_col--;
            vp->current_col--;
//            textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (textmode_current_row * textmode_width) + textmode_current_col;
            textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (vp->current_row * vp->width) + vp->current_col;
            *textmode_memptr = ' ' | TEXTMODE_ATTR(vp);
        }
    }
    else if (c >= ' ') {
//       textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (textmode_current_row * textmode_width) + textmode_current_col;
       textmode_memptr = (u16 *)TEXTMODE_BASE_MEM + (vp->current_row * vp->width) + vp->current_col;
       *textmode_memptr = c | TEXTMODE_ATTR(vp);

//        if (++textmode_current_col >= textmode_width) {
        if (++vp->current_col > vp->end_col) {
//            textmode_current_row++;
            vp->current_row++;
//            textmode_current_col = 0;
            vp->current_col = 0;
        }
    }
 
//    if (textmode_current_row >= textmode_height) {
    if (vp->current_row > vp->end_row) {
//        textmode_scroll();
        textmode_scroll_vp( vp );
//        textmode_current_row = textmode_height - 1;
        vp->current_row = vp->end_row;
    }
}


void textmode_putc( char c ) {
    textmode_putc_vp( &root_vp, c );
}



void textmode_putc_at_vp( b8000_textmode_viewport* vp, char c, u8 row, u8 col ) {
//    if (row >= textmode_height || col >= textmode_width)
    if (row > vp->end_row || col > vp->end_col)
        return;

//    u8 hold_row = textmode_current_row;
    u8 hold_row = vp->current_row;
//    u8 hold_col = textmode_current_col;
    u8 hold_col = vp->current_col;

//    textmode_current_row = row;
    vp->current_row = row;
//    textmode_current_col = col;
    vp->current_col = col;

    textmode_putc( c );

//    textmode_current_row = hold_row;
    vp->current_row = hold_row;
//    textmode_current_col = hold_col;
    vp->current_col = hold_col;
}


void textmode_putc_at( char c, u8 row, u8 col ) {
    textmode_putc_at_vp( &root_vp, c, row, col );
}


void textmode_puts_vp( b8000_textmode_viewport* vp, char *s ) {
    while (*s)
//        textmode_putc( *s++ );
        textmode_putc_vp( vp, *s++ );
}


void textmode_puts( char *s ) {
    textmode_puts_vp( &root_vp, s );
}


void textmode_puts_at_vp( b8000_textmode_viewport* vp, char* s, u8 row, u8 col ) {
//    if (row >= textmode_height || col >= textmode_width)
    if (row > vp->end_row || col > vp->end_col)
        return;

//    u8 hold_row = textmode_current_row;
    u8 hold_row = vp->current_row;
//    u8 hold_col = textmode_current_col;
    u8 hold_col = vp->current_col;

//    textmode_current_row = row;
    vp->current_row = row;
//    textmode_current_col = col;
    vp->current_col = col;

//    textmode_puts( s );
    textmode_puts_vp( vp, s );

//    textmode_current_row = hold_row;
    vp->current_row = hold_row;
//    textmode_current_col = hold_col;
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

//    textmode_puts( (char*)(int_buff + last_digit) );
    textmode_puts_vp( vp, (char*)(int_buff + last_digit) );
}


void textmode_puti( unsigned int i ) {
    textmode_puti_vp( &root_vp, i );
}



void textmode_puti_at_vp( b8000_textmode_viewport* vp, unsigned int i, u8 row, u8 col ) {
//    if (row >= textmode_height || col >= textmode_width)
    if (vp->current_row > vp->end_row || vp->current_col > vp->end_col)
        return;

//    u8 hold_row = textmode_current_row;
    u8 hold_row = vp->current_row;
//    u8 hold_col = textmode_current_col;
    u8 hold_col = vp->current_col;

//    textmode_current_row = row;
    vp->current_row = row;
//    textmode_current_col = col;
    vp->current_col = col;

    textmode_puti( i );

//    textmode_current_row = hold_row;
    vp->current_row = hold_row;
//    textmode_current_col = hold_col;
    vp->current_col = hold_col;
}


/* just like textmode_puti, but at a particular row and column */
void textmode_puti_at( unsigned int i, u8 row, u8 col ) {
    textmode_puti_at_vp( &root_vp, i, row, col );
}




void textmode_cls_vp( b8000_textmode_viewport* vp ) {
    u16 i;

    textmode_memptr = (u16 *)TEXTMODE_BASE_MEM;

//    for (i = 0; i < textmode_height * textmode_width; i++)
    for (i = vp->start_row; i <= vp->end_row * vp->width; i++)
        *textmode_memptr++ = ' ' | TEXTMODE_ATTR(vp);

//    textmode_current_col = 0;
    vp->current_col = 0;
//    textmode_current_row = 0;
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
