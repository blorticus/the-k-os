#include <video/b8000textmode.h>
#include <sys/types.h>

void textmode_simple_putc( char c, _U16 row, _U16 col ) {
    _U16* memptr;

    memptr = (_U16 *)TEXTMODE_BASE_MEM + row * 80 + col;
    *memptr = 0x0f00 | (_U16)c;
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
