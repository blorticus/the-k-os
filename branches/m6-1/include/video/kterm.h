#ifndef __VIDEO_KTERM_H__
#define __VIDEO_KTERM_H__

/**
 * BRIEF:           A very basic windowed terminal for use by the kernel
 * BACKGROUND:      Provides the kernel (and kernel level threads) a basic VGA terminal for output.
 * IMPLEMENTATION:  The terminal uses VBA basic color mode.  It is tiled, allowing sections of the rendering
 *                  area to scroll independently.
 **/


#include <sys/types.h>
#include <stdio.h>


/**
 *
 * DESCRIPTION:     A kterm tile
 * ELEMENTS:        width           - the width of the tile
 *                  height          - the height of the tile
 *                  base_offset     - imagining that the rendering area is a matrix of characters, this is the offset of the first
 *                                    character from the upper left corner of the screen
 *                  end_offset      - imagining that the rendering area is a matrix of characters, this is the offset of the last
 *                                    character from the upper left corner of the screen
 *                  current_offset  - imagining that the rendering area is a matrix of characters, this is the offset of the last
 *                                    character written from the upper left corner of the screen
 *                  colors          - the foreground/background color palette currently in use (fg upper 4 bits, bg lower 4 bits)
 */ 

typedef struct kterm_window {
    u8 width;
    u8 height;
    u16 base_offset;
    u16 end_offset;
    u16 current_offset;
    u8 colors;
} kterm_window;

typedef kterm_window* KTERM_WINDOW;


/**
 *
 * DESCRIPTION:     Initialize the kterm system.  Generally this must be done before subsequent use of kterm_* methods.
 *                  'screen_height' and 'screen_width' are the height and width of the VGA basic color textmode rendering areas.
 * RETURN:          void
 * SIDE-EFFECTS:    -
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */ 
void kterm_init( u8 screen_height, u8 screen_width );


/**
 *
 * DESCRIPTION:     Create a kterm tile starting at offset 'window_start' (this has the same interpretation as base_offset
 *                  of kterm_window above).  The tile will have height 'window_height' and width of 'window_width'. 
 * RETURN:          void
 * SIDE-EFFECTS:    'w' is provided sensible values.  In general, it should be passed around to other kterm_window_*
 *                  methods, but its internal members shouldn't be directly altered.
 * NOTES:           No effort is made to ensure that window_start, window_height and window_width make any sense.  Moreover,
 *                  kterm pretty much expects that tiles are rectangular, so if the window_start makes this untrue (that is,
 *                  if it does not represent a character slot in the left rendering column), the results are undefined.
 * RE-ENTRANT?:     NO
 *
 */ 
void kterm_create_window( KTERM_WINDOW w, u16 window_start, u8 window_height, u8 window_width );


/**
 *
 * DESCRIPTION:     The kterm "root window" is the tile that takes up the entire viewing area.  Notice that tiles
 *                  can be set atop one another.  This method makes a copy of the structure representing the root
 *                  window
 * RETURN:          void
 * SIDE-EFFECTS:    'w' is provided sensible values.
 * NOTES:           -
 * RE-ENTRANT?:     NO
 *
 */ 
void kterm_dup_root_window( KTERM_WINDOW w );


/**
 *
 * DESCRIPTION:     Retrieve a pointer to the actual root window
 * RETURN:          A pointer to the root window (see kterm_dup_root_window above)
 * SIDE-EFFECTS:    -
 * NOTES:           It's not really a good idea to use the base root window object, so don't call this method, ok?
 * RE-ENTRANT?:     YES (though, technically, the root window pointer could be updated, in which case, the results are undefined)
 *
 */ 
KTERM_WINDOW kterm_get_root_window( void );


/**
 *
 * DESCRIPTION:     Clear the root window by writing a space (in the selected color palette) at every position within the tile
 * RETURN:          A pointer to the root window (see kterm_dup_root_window above)
 * SIDE-EFFECTS:    The next character rendering slot (e.g., for kterm_window_putc()) is the first column of the first row.
 * NOTES:           If tiles are overlaid, this will erase part (or all, depending on the amount of overlay) of the overlaid tile(s)
 * RE-ENTRANT?:     YES (though if tiles are overlaid and something is written to an overlaid tile, there will be a race-condition)
 *
 */ 
void kterm_window_cls( KTERM_WINDOW w );


/**
 *
 * DESCRIPTION:     Scroll 'w' one line.  This eliminates all characters in the first line, and moves each character to one row
 *                  above its current location.  The next rendering slot will be the first column of the last line.
 * RETURN:          void
 * SIDE-EFFECTS:    Characters move as described
 * NOTES:           If tiles are overlaid, this will scroll part (or all, depending on the amount of overlay) of the overlaid tile(s)
 * RE-ENTRANT?:     YES (though if tiles are overlaid and something is written to an overlaid tile, there will be a race-condition)
 *
 */ 
void kterm_window_scroll( KTERM_WINDOW w );


/**
 *
 * DESCRIPTION:     Put a character in a kterm tile at the next rendering slot.  Normally, when a tile is created, the rendering
 *                  slot starts at left column, in the first row, and is advanced on any write.
 * RETURN:          void
 * SIDE-EFFECTS:    -
 * NOTES:           If tiles are overlaid, this will affect the underlying tile(s) if they overlap at the current rendering character slot.
 * RE-ENTRANT?:     YES (though if tiles are overlaid and something is written to an overlaid tile, there will be a race-condition)
 *
 */ 
void kterm_window_putc( KTERM_WINDOW w, char c );


/**
 *
 * DESCRIPTION:     Write a null-terminated string to a kterm tile 'w'
 * RETURN:          void
 * SIDE-EFFECTS:    -
 * NOTES:           If tiles are overlaid, this will erase part (or all, depending on the amount of overlay) of the overlaid tile(s).  If
 *                  's' is not null-terminated, the results are undefined (but probably really bad)
 * RE-ENTRANT?:     YES (though if tiles are overlaid and something is written to an overlaid tile, there will be a race-condition)
 *
 */ 
void kterm_window_puts( KTERM_WINDOW w, const char* s );


/**
 *
 * DESCRIPTION:     Given an unsigned integer 'i', write its value as decimal in a kterm tile
 * RETURN:          void
 * SIDE-EFFECTS:    -
 * NOTES:           If tiles are overlaid, this will erase part (or all, depending on the amount of overlay) of the overlaid tile(s)
 * RE-ENTRANT?:     YES (though if tiles are overlaid and something is written to an overlaid tile, there will be a race-condition)
 *
 */ 
void kterm_window_puti( KTERM_WINDOW w, unsigned int i );


/**
 *
 * DESCRIPTION:     Given an 32-bit unsigned integer, write its value as hex (without leading 0x and all characters lower-case)
 *                  to a kterm tile
 * RETURN:          void
 * SIDE-EFFECTS:    -
 * NOTES:           If tiles are overlaid, this will erase part (or all, depending on the amount of overlay) of the overlaid tile(s)
 * RE-ENTRANT?:     YES (though if tiles are overlaid and something is written to an overlaid tile, there will be a race-condition)
 *
 */ 
void kterm_window_puth( KTERM_WINDOW w, u32 i );


/**
 *
 * DESCRIPTION:     Read characters typed in, and echo them to a kterm tile, 'w'.  Also, write the character to the buffer 'buffer'.
 *                  Continue collecting (and echoing) characters until a newline or 'size' number of characters have been entered.
 * RETURN:          -
 * SIDE-EFFECTS:    -
 * NOTES:           If tiles are overlaid, this will erase part (or all, depending on the amount of overlay) of the overlaid tile(s).
 *                  If 'size' is larger than the allocation for 'buffer', the results are undefined.
 * RE-ENTRANT?:     NO
 *
 */ 
void kterm_window_readline( KTERM_WINDOW w, char* buffer, unsigned int size );



/**
 *
 * DESCRIPTION:     Taken together, these two methods provide a printf() function (using the cprintf() semantics) for a kterm tile, 'w',
 * RETURN:          The number of characters written
 * SIDE-EFFECTS:    Characters are written to the kterm tile 'w'
 * NOTES:           If tiles are overlaid, this will erase part (or all, depending on the amount of overlay) of the overlaid tile(s)
 * RE-ENTRANT?:     YES (though if tiles are overlaid and something is written to an overlaid tile, there will be a race-condition)
 *
 */ 
void kterm_window_putchar_pf( int c, char* params );

//#define kterm_printf(fmt, ...) cprintf((void*)kterm_putchar, fmt, ## __VA_ARGS__)
#define kterm_window_printf(w,fmt, ...) cprintf((void*)kterm_window_putchar_pf, (char*)w, fmt, ## __VA_ARGS__)

#endif
