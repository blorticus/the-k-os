#ifndef __B8000_TEXTMODE_H__
#define __B8000_TEXTMODE_H__

#include <sys/types.h>

/**
 * BRIEF:           Provides "driver" for 0xb8000 text mode
 * BACKGROUND:      IA-32 systems that provide a VGA subsystem will read the physical memory starting at
 *                  0xb8000 and print characters to the screen based on what's there.  The memory is a
 *                  a matrix of 16-bit words.  The upper half specifies the background color and the foreground
 *                  color for the character, using a 16 color (8-bits each for fg and bg) palette.  The lower
 *                  half is the 8-bit ASCII character.
 * IMPLEMENTATION:  This package uses row and column to keep track of where the last character was written,
 *                  and when another character is written, it advances the column pointer.  When the pointer
 *                  moves past the width of the screen (which is currently statically set), it advances the row
 *                  pointer.  When the row pointer move beyond the height (also statically set), it scrolls the
 *                  screen, moving everyhing "up" one row (the first row is lost), and inserting blanks in the
 *                  last row.
 * RE-ENTRANT?:     NO
 **/

/*********
 * Enumerations
 **/

/* 8-bit color palette for foreground and background colors */
enum TEXTMODE_COLOR
{
    black, blue, green, cyan, red, magenta, brown,
    light_gray, dark_gray, light_blue, light_green,
    light_cyan, light_red, light_magenta, yellow, white
};


typedef struct b8000_textmode_viewport {
    u8 start_col;
    u8 end_col;
    u8 width;
    u8 start_row;
    u8 end_row;
    u8 height;
    u8 current_row;
    u8 current_col;
    u8 color;
} b8000_textmode_viewport;


/*********
 * Public methods
 **/

/* initialize video mode, setting screen to width x height, initializing pointer to at_row and at_col
 * (both start at zero, and can be width - 1 or height - 1) and setting the character color to bgcolor
 * (background) and fgcolor (foreground) */
void textmode_init( u8 width, u8 height, u8 at_row, u8 at_col, u8 bgcolor, u8 fgcolor );


/* default initalization.  Sets screen to 80 x 25, row and column to 0 (upper left) and light_gray on black */
void textmode_init_default( void );


/* sets the location for the next character written.  row and column start at zero and may be up to height - 1 or
 * width - 1, respectively */
void textmode_set_location( u8 row, u8 column );


/* put a character on the screen using the configured fg and bg colors, and the current row and column, then
 * advance column by one (which may cause row to advance and reset column to zero.  May also cause scrolling) */
void textmode_putc( char c );
void textmode_putc_vp( b8000_textmode_viewport* vp, char c );


/* put a character on the screen using the configured fg and bg colors at the named row and column.  After, the
 * current row and current column pointers are returned to their original location from before the call to this
 * method.  Be aware that if this character put forces a scroll, the current row pointer is returned
 * to its absolute position from before the scroll, meaning they will actually point one line before, lexically.
 * If the supplied row is greater than the screen height, or the column is greater than the screen width, this
 * method silently returns */
void textmode_putc_at( char c, u8 row, u8 column );


/* put a string to the screen using the configured fg and bg colors, starting at the current row and column, then
 * advance column by length of string (see textmode_putc()) */
void textmode_puts( char *s );


/* just like textmode_putc_at(), except it puts an entire string.  The same caveats about pointers and provided
 * row/column apply */
void textmode_puts_at( char* s, u8 row, u8 column );


/* put an unsigned integer */
void textmode_puti( unsigned int i );


/* just like textmode_puti, but at a particular row and column */
void textmode_puti_at( unsigned int i, u8 row, u8 column );


/* clear the screen (actually, fill it with spaces in the current fg/bg colors) */
void textmode_cls( void );
void textmode_cls_vp( b8000_textmode_viewport* vp );


/* convert unsigned byte into two hex digits and textmode_putc() them */
void textmode_put_hexbyte( u8 byte );


/* like libc putchar() */
int textmode_putchar( int c );


/*********
 * Testing methods
 **/
void textmode_put_dec( unsigned int n ); /* convert int to string output */



#endif
