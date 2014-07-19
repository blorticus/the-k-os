#ifndef _VIDEO_TEXT_TERMINAL_
#define _VIDEO_TEXT_TERMINAL_

#include <video/lfb.h>
#include <sys/types.h>


typedef struct {
    frame_buffer*           fb;
    u8                      font_char_pixel_width;
    u8                      font_char_pixel_height;
    u8*                     font_def;
    u32                     next_char_x_pos;
    u32                     next_char_y_pos;
    u16                     text_cols;
    u16                     text_rows;
    u32                     fg_color;
    u32                     bg_color;
} term_entry;


typedef enum {
    TE_NoError          = 0,
    TE_InvalidCharacter = 1,
    TE_InvalidLocation  = 2
} term_error;


/**
 * DESCRIPTION:
 *      Initialize a frame buffer text terminal
 * ARGS:
 *      - te                        : term_entry pointer
 *      - fb                        : frame_buffer pointer
 *      - font_char_pixel_width     : the number of horizontal pixels in each fixed font character
 *      - font_char_pixel_height    : the number of vertical pixels in each fixed font character
 *      - font_def                  : matrix of [font_char_pixel_width][font_char_pixel_height] providing bitmap for each char 0..127
 * RETURNS:
 *      0 if no error or non-zero otherwise
 **/
term_error term_init( term_entry* te, frame_buffer* fb, u8 font_char_pixel_width, u8 font_char_pixel_height, u8* font_def );


/**
 * DESCRIPTION:
 *      x and y bounds of text terminal space (that is, [x] characters fit horizontally
 *      and [y] rows of characters)
 * ARGS:
 *      none
 * RETURNS:
 *      The number of columns or rows
 **/
u16 term_text_columns( term_entry* te );
u16 term_text_rows( term_entry* te );


/**
 * DESCRIPTION:
 *      Put a character at a specific location in the text terminal space
 * ARGS:
 *      - c             : the character, rendered from the initialized font
 *      - at_x          : render the character at x position in text terminal space
 *      - at_y          : render the character at y position in text terminal space
 * RETURNS:
 *      0 if no error, TE_InvalidCharacter if 'c' is negative, TE_InvalidLocation if at_x or at_y is
 *      outside of the terminal text bounds.  Character next position is at_x + 1 (which may set x to 0,
 *      the row may advance)
 **/
term_error term_putchar_at( term_entry* te, char c, u16 at_x, u16 at_y );


/**
 * DESCRIPTION:
 *      Put a character at the next location (starts at 0,0, advances horizontally).  Tab represented by
 *      four spaces, \n by advancing to start of next row, \r by advancing to first column of current row,
 *      \b by moving to previous column (and possibly, previous row) but not before 0,0, '\0' is not
 *      printed, other non-printables also not printed
 * ARGS:
 *      - c             : the character, rendered from the initialized font
 * RETURNS:
 *      0 if no error, TE_InvalidCharacter if 'c' is negative
 **/
term_error term_putchar( term_entry* te, char c );


/**
 * DESCRIPTION:
 *      Put a character sequence starting at provided x,y
 * ARGS:
 *      - s             : the character sequence, rendered from the initialized font
 *      - at_x          : render the character at x position in text terminal space
 *      - at_y          : render the character at y position in text terminal space
 * RETURNS:
 *      0 if no error, TE_InvalidCharacter if any char is negative, TE_InvalidLocation if at_x or at_y is
 *      outside of the terminal text bounds
 **/
term_error term_puts_at( term_entry* te, const char* s, u16 at_x, u16 at_y );


/**
 * DESCRIPTION:
 *      Put a character sequence starting at the next location, with the same rules as term_putchar,
 *      terminating at '\0' (which isn't printed)
 * ARGS:
 *      - s             : the character sequence, rendered from the initialized font
 *      - at_x          : render the character at x position in text terminal space
 *      - at_y          : render the character at y position in text terminal space
 * RETURNS:
 *      0 if no error, TE_InvalidCharacter if any char is negative
 **/
term_error term_puts( term_entry* te, const char* s );


#endif
