#include <vga.h>
#include <sys/types.h>

#define VGA_BASE_MEM    0xB8000

vga_t vga_init( void ) {
    vga_t v;

    v.width = 80;
    v.height = 40;
    v.current_row = v.current_col = 0;
    v.color = 0x0007;       /* ltgray on black */

    v.memptr = (_U16 *)VGA_BASE_MEM; /* 2 bytes for each vga text slot entry, hence _U16 */

    return v;
}


void vga_set_location( _U16 row, _U16 column, vga_t vga ) {
    if (row >= vga.height || column >= vga.width)
        return;

    vga.current_row = row;
    vga.current_col = column;

    vga.memptr = (_U16 *)VGA_BASE_MEM + (row * vga.width) + column;
}


void vga_putc( char c, vga_t vga ) {
    if (c == '\r') {
        vga.current_col = 0;
    }
    else if (c == '\n') {
        vga.current_col = 0;
        vga.current_row++;
    }
    else if (c >= ' ') {
        *vga.memptr = c | vga.color;
        vga.current_col++;
    }

    if (vga.current_col >= vga.width) {
        vga.current_row++;
        vga.current_col = vga.current_col % vga.width;
    }
    if (vga.current_row >= vga.height) {
        vga_scroll( vga );
        vga.current_row = vga.height - 1;
    }

    vga.memptr = (_U16 *)VGA_BASE_MEM + (vga.current_row * vga.width) + vga.current_row;
}


void vga_puts( char *s, vga_t vga ) {
    while (*s)
        vga_putc( *s++, vga );
}


void scroll( vga_t vga ) {
    _U16 i;

    _U16* dst_ptr = (_U16 *)VGA_BASE_MEM;
    _U16* src_ptr = (_U16 *)VGA_BASE_MEM + vga.width;

    /* inefficient 2byte-by-2byte copy */
    for (i = 1; i < (vga.height - 2) * vga.width; i++)
        *dst_ptr++ = *src_ptr++;

    for (i = 0; i < vga.width - 1; i++)
        *dst_ptr = ' ' | vga.color;
}

