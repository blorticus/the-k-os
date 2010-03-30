#ifndef __VIDEO_VGA_H__
#define __VIDEO_VGA_H__

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

/**
 *
 * DESCRIPTION:     VGA basic color textmode provides a 4-bit color palette for foreground and background
 *                  These correspond to the defined colors and their bit values.
 */ 
enum TEXTMODE_COLOR
{
    black, blue, green, cyan, red, magenta, brown,
    light_gray, dark_gray, light_blue, light_green,
    light_cyan, light_red, light_magenta, yellow, white
};


/* when unit testing, generally, writing directly to b8000 is not possible.  The following methods
   can be overridden to allow sensible testing (allowing writes to somewhere other than b8000 */
#ifdef TEST
    extern void TEST_b8000_write_char_at( u16 offset, u16 colors, char c );
    extern void* TEST_b8000_derive_screen_base_ptr( void );
    #define M_B8000_write_char_at(offset,colors,c) (TEST_b8000_write_char_at( offset, colors, c ))
    #define TEXTMODE_BASE_MEM   (TEST_b8000_derive_screen_base_ptr())
#else
    #define M_B8000_write_char_at(offset,colors,c) (*((u16*)((u16*)(0xB8000) + offset)) = ((u16)(colors << 8)) | (u16)c)
    #define TEXTMODE_BASE_MEM    0xB8000
#endif


/**
 *
 * DESCRIPTION:     Copy a chunk of vga basic color textmode memory.  Generally, this facilitates scrolling.
 *                  Imagining that textmode memory is a flat matrix of characters on an 80 x 25 screen (and
 *                  thus an 80x25 == 2000 element array of characters), 'first_pos' is the element of the
 *                  character array where copying should begin.  'last_pos' is the last position that should
 *                  be copied.  'copy_back' is the number of slots "backward" that the copy should be made.
 *                  Thus, if first_pos == 20, last_pos == 40 and copy_back == 10, the character (and color
 *                  attributes) and b8000 + 20 slot will be copied to b8000 + 10 slot, b8000 + 21 slot will
 *                  be copied to b8000 + 11 slot, and so forth, through b8000 + 40 slot (inclusive) -- which
 *                  is copied to b8000 + 30 slot.
 * RETURN:          void
 * SIDE-EFFECTS:    Video memory is updated
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */ 
void textmode_copy_back( u16 first_pos, u16 last_pos, u16 copy_back );



/**
 *
 * DESCRIPTION:     Each VGA slot is a character (lower half) and an attribute (upper half).  This
 *                  macro take a foreground color 'fgcolor' and a background color 'bgcolor', and
 *                  produces an upper half word with the appropriate values.  fgcolor and bgcolor
 *                  come from TEXTMODE_COLOR above.
 * RETURN:          void
 * SIDE-EFFECTS:    -
 * NOTES:           -
 * RE-ENTRANT?:     -
 *
 */ 
#define make_b8000_colors( fgcolor, bgcolor ) ((u8)((bgcolor << 4) | fgcolor))


#endif
