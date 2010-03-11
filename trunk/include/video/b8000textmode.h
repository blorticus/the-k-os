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


#ifdef TEST
    extern void TEST_b8000_write_char_at( u16 offset, u16 colors, char c );
    extern void* TEST_b8000_derive_screen_base_ptr( void );
    #define M_B8000_write_char_at(offset,colors,c) (TEST_b8000_write_char_at( offset, colors, c ))
    #define TEXTMODE_BASE_MEM   (TEST_b8000_derive_screen_base_ptr())
#else
    #define M_B8000_write_char_at(offset,colors,c) (*((u16*)((u16*)(0xB8000) + offset)) = ((u16)(colors << 8)) | (u16)c)
    #define TEXTMODE_BASE_MEM    0xB8000
#endif


void textmode_copy_back( u16 first_pos, u16 last_pos, u16 copy_back );

#define make_b8000_colors( fgcolor, bgcolor ) ((u8)((bgcolor << 4) | fgcolor))


#endif
