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


typedef struct b8000_textmode_window {
    u8 width;
    u16 attrs;
    u16* pos;
    u16* first_pos;
    u16* last_pos;
} B8000_TEXTMODE_WINDOW;


/*********
 * Public methods
 **/

void init_textmode_window( B8000_TEXTMODE_WINDOW* w, u8 start_row, u8 window_height, u8 window_width, u8 colors );
void textmode_window_scroll( B8000_TEXTMODE_WINDOW* w );
void textmode_window_putc( B8000_TEXTMODE_WINDOW* w, unsigned char c );
void textmode_window_set_pos( B8000_TEXTMODE_WINDOW* w, u8 row, u8 col );
void textmode_window_cls( B8000_TEXTMODE_WINDOW* w );
void textmode_window_puts( B8000_TEXTMODE_WINDOW* w, char* s );



/* initialize video mode, setting screen to width x height, initializing pointer to at_row and at_col
 * (both start at zero, and can be width - 1 or height - 1) and setting the character color to bgcolor
 * (background) and fgcolor (foreground) */
void textmode_init( u8 width, u8 height, u8 at_row, u8 at_col, u8 bgcolor, u8 fgcolor );


/* default initalization.  Sets screen to 80 x 25, row and column to 0 (upper left) and light_gray on black */
void textmode_init_default( void );


/* put a character on the screen using the configured fg and bg colors, and the current row and column, then
 * advance column by one (which may cause row to advance and reset column to zero.  May also cause scrolling) */
void textmode_putc( char c );


/* put a string to the screen using the configured fg and bg colors, starting at the current row and column, then
 * advance column by length of string (see textmode_putc()) */
void textmode_puts( char *s );


/* put an unsigned integer */
void textmode_puti( unsigned int i );


/* clear the screen (actually, fill it with spaces in the current fg/bg colors) */
void textmode_cls( void );


/* convert unsigned byte into two hex digits and textmode_putc() them */
void textmode_put_hexbyte( u8 byte );


/* like libc putchar() */
int textmode_putchar( int c );

void textmode_scroll( void );


/*********
 * Testing methods
 **/
void textmode_put_dec( unsigned int n ); /* convert int to string output */



#endif
