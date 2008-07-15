#ifndef __B8000_TEXTMODE_H__
#define __B8000_TEXTMODE_H__

#include <sys/types.h>

/**
 * Enumerations
 **/
enum TEXTMODE_COLOR
{
    black, blue, green, cyan, red, magenta, brown,
    light_gray, dark_gray, light_blue, light_green,
    light_cyan, light_red, light_magenta, yellow, white
};



/**
 * Public methods
 **/
void textmode_init( _U8 width, _U8 height, _U8 at_row, _U8 at_col, _U8 bgcolor, _U8 fgcolor );
void textmode_default_init( void );
void textmode_set_location( _U8 row, _U8 column );
void textmode_putc( char c );
void textmode_puts( char *s );
void textmode_cls( void );


/**
 * Testing methods
 **/
void textmode_put_dec( unsigned int n ); /* convert int to string output */


/**
 * Internal methods
 **/
void textmode_scroll( void );
void textmode_simple_putc( char c, _U16 row, _U16 col );


#endif
