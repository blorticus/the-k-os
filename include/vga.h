#ifndef __VGA_H__
#define __VGA_H__

#include <sys/types.h>


/*
 * Basic struct for vga info:
 */
typedef struct vga_t {
    _U16 width;         /* total width for vga display */
    _U16 height;        /* total height for vga display */

    _U16 current_row;   /* current cursor location, row */
    _U16 current_col;   /*               ... and column */

    _U8 color;          /* VGA attributes */

    _U16* memptr;       /* VGA pointer */
} vga_t;


void vga_init( void );
void vga_set_location( _U8 row, _U8 column );
void vga_putc( char c );
void vga_puts( char *s );
void vga_cls( void );

void vga_scroll( void );
void vga_simple_putc( char c, _U16 row, _U16 col );


#endif
