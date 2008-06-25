#include <vga.h>
#include <sys/types.h>

#define VGA_BASE_MEM    0xB8000

vga_t KERNEL_VGA;


void vga_simple_putc( char c, _U16 row, _U16 col ) {
    _U16* memptr;

    memptr = (_U16 *)VGA_BASE_MEM + row * 80 + col;
    *memptr = c | 0x0007;
}


void vga_init( void ) {
    KERNEL_VGA.width = 80;
    KERNEL_VGA.height = 40;
    KERNEL_VGA.current_row = KERNEL_VGA.current_col = 0;
    KERNEL_VGA.color = 0x0007;       /* ltgray on black */

    KERNEL_VGA.memptr = (_U16 *)VGA_BASE_MEM; /* 2 bytes for each vga text slot entry, hence _U16 */
}


void vga_set_location( _U16 row, _U16 column ) {
    if (row >= KERNEL_VGA.height || column >= KERNEL_VGA.width)
        return;

    KERNEL_VGA.current_row = row;
    KERNEL_VGA.current_col = column;

    KERNEL_VGA.memptr = (_U16 *)VGA_BASE_MEM + (row * KERNEL_VGA.width) + column;
}


void vga_putc( char c ) {
    if (c == '\r') {
        KERNEL_VGA.current_col = 0;
    }
    else if (c == '\n') {
        KERNEL_VGA.current_col = 0;
        KERNEL_VGA.current_row++;
    }
    else if (c >= ' ') {
        *KERNEL_VGA.memptr = c | KERNEL_VGA.color;
        KERNEL_VGA.current_col++;
    }

    if (KERNEL_VGA.current_col >= KERNEL_VGA.width) {
        KERNEL_VGA.current_row++;
        KERNEL_VGA.current_col = KERNEL_VGA.current_col % KERNEL_VGA.width;
    }
    if (KERNEL_VGA.current_row >= KERNEL_VGA.height) {
        vga_scroll();
        KERNEL_VGA.current_row = KERNEL_VGA.height - 1;
    }

    KERNEL_VGA.memptr = (_U16 *)VGA_BASE_MEM + (KERNEL_VGA.current_row * KERNEL_VGA.width) + KERNEL_VGA.current_row;
}


void vga_puts( char *s ) {
    while (*s)
        vga_putc( *s++ );
}


void vga_scroll( void ) {
    _U16 i;

    _U16* dst_ptr = (_U16 *)VGA_BASE_MEM;
    _U16* src_ptr = (_U16 *)VGA_BASE_MEM + KERNEL_VGA.width;

    /* inefficient 2byte-by-2byte copy */
    for (i = 1; i < (KERNEL_VGA.height - 2) * KERNEL_VGA.width; i++)
        *dst_ptr++ = *src_ptr++;

    for (i = 0; i < KERNEL_VGA.width - 1; i++)
        *dst_ptr = ' ' | KERNEL_VGA.color;
}

