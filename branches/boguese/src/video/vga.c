#include <video/vga.h>
#include <sys/types.h>
#include <platform/ia-32/asm.h>

void textmode_simple_putc( char c, u16 row, u16 col ) {
    u16* memptr;

    memptr = (u16 *)TEXTMODE_BASE_MEM + row * 80 + col;
    *memptr = 0x0f00 | (u16)c;
}


/*
 * Starting at first_pos (offset from start of textmode memory) copy characters through last_pos back copy_back
 * positions.  If copy_back > first_pos , or first_pos > last_pos, silently return.
 */
void textmode_copy_back( u16 first_pos, u16 last_pos, u16 copy_back ) {
    if (copy_back > first_pos)
        return;

    u16* dest = (u16*)((u16*)(TEXTMODE_BASE_MEM) + first_pos - copy_back);
    u16* src  = (u16*)((u16*)(TEXTMODE_BASE_MEM) + first_pos);

    while (first_pos++ <= last_pos)
        *dest++ = *src++;
}


/* XXX: these can actually be 3b4/5 or 3d4/5.  I'll have to sweep through later to implement code for detecting
   which.  See http://www.osdever.net/FreeVGA/vga/crtcreg.htm#0A */
#define CRTC_ADDRESS_REGISTER       0x3d4
#define CRTC_DATA_REGISTER          0x3d5

/* cursor start.  Bits are (from least significant): [0-4] Cursor Scan Start Line; [5] Cursor Disable [6-7] zeroes */
#define VGA_CURSOR_START_REGISTER_ADDRESS       0x0a

void vga_disable_hardware_cursor( void ) {
    /* scan start line should be irrelevant if cursor is disabled */
    ioport_writeb( CRTC_ADDRESS_REGISTER, VGA_CURSOR_START_REGISTER_ADDRESS );
    ioport_writeb( CRTC_DATA_REGISTER, 0x20 );
}
