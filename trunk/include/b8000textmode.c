#ifndef __VGA_H__
#define __VGA_H__

#include <sys/types.h>

/**
 * Enumerations
 **/
enum VGA_COLOR
{
    black, blue, green, cyan, red, magenta, brown,
    light_gray, dark_gray, light_blue, light_green,
    light_cyan, light_red, light_magenta, yellow, white
};



/**
 * Public methods
 **/
void vga_init( _U8 width, _U8 height, _U8 at_row, _U8 at_col, _U8 bgcolor, _U8 fgcolor );
void vga_default_init( void );
void vga_set_location( _U8 row, _U8 column );
void vga_putc( char c );
void vga_puts( char *s );
void vga_cls( void );


/**
 * Testing methods
 **/
void vga_put_dec( unsigned int n ); /* convert int to string output */


/**
 * Internal methods
 **/
void vga_scroll( void );
void vga_simple_putc( char c, _U16 row, _U16 col );


#endif
