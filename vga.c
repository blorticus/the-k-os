#include <vga.h>
#include <sys/types.h>

/*
 * RE-ENTRANT?      NO 
 */

#define VGA_BASE_MEM    0xB8000


void vga_simple_putc( char c, _U16 row, _U16 col ) {
    _U16* memptr;

    memptr = (_U16 *)VGA_BASE_MEM + row * 80 + col;
    *memptr = 0x0f00 | (_U16)c;
}


_U8 vga_color;
_U8 vga_width;
_U8 vga_height;
_U8 vga_current_row;
_U8 vga_current_col;

_U16* vga_memptr;

#define VGA_ATTR ((_U16)(vga_color << 8));


void vga_init( void ) {
    vga_width        = 80;
    vga_height       = 24;
    vga_current_row  = 0;
    vga_current_col  = 0;
    vga_color        = 0x07;       /* ltgray on black */
}




void vga_set_location( _U8 row, _U8 column ) {
    if (row >= vga_height || column >= vga_width)
        return;

    vga_current_row = row;
    vga_current_col = column;
}


void vga_putc( char c ) {
    vga_memptr = (_U16 *)VGA_BASE_MEM + (vga_current_row * vga_width) + vga_current_col;
    *vga_memptr = c | VGA_ATTR;
    vga_current_col++;
     
//     if (c == '\r') {
//         vga_current_col = 0;
//     }
//     else if (c == '\n') {
//         vga_current_col = 0;
//         vga_current_row++;
//     }
//     else if (c >= ' ') {
//        vga_memptr = (_U16 *)VGA_BASE_MEM + (vga_current_row * vga_width) + vga_current_col;
//        *vga_memptr = c | VGA_ATTR;

//         if (++vga_current_col >= vga_width) {
//             vga_current_row++;
//             vga_current_col = 0;
//         }
//     }
// 
// //    if (vga_current_row >= vga_height) {
// //        vga_scroll();
// //        vga_current_row = vga_height - 1;
// //    }
}


void vga_puts( char *s ) {
    while (*s)
        vga_putc( *s++ );
}


void vga_scroll( void ) {
    _U16 i;

    _U16* dst_ptr = (_U16 *)VGA_BASE_MEM;
    _U16* src_ptr = (_U16 *)VGA_BASE_MEM + vga_width;

    /* inefficient 2byte-by-2byte copy */
    for (i = 1; i < (vga_height - 2) * vga_width; i++)
        *dst_ptr++ = *src_ptr++;

    for (i = 0; i < vga_width - 1; i++)
        *dst_ptr = ' ' | VGA_ATTR;
}



void vga_cls( void ) {
//    _U16 i;
//
//    vga_memptr = (_U16 *)VGA_BASE_MEM;
//
//    for (i = 0; i < vga_height * vga_width; i++)
//        *vga_memptr = ' ' | VGA_ATTR;
//
//    vga_current_col = 0;
//    vga_current_row = 0;
}
