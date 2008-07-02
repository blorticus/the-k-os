//#include <vga.h>
#include <sys/types.h>

void show_32_register( char a, char b, char c, int row, int col, _U32 reg_value );
void show_16_register( char a, char b, int row, int col, _U16 reg_value );

int get_eip( void ) {
   _U32 eip;

    asm volatile ( "movl -4(%%ebp), %0": "=r"(eip): );

    return eip;
}

void kmain( void ) {
    _U32 reg32;
    _U16 reg16;

    asm( "mov %%ss, %0;" :"=r"(reg16) : /* no input */ );
    show_16_register( 's', 's', 5, 0, reg16 );

    asm( "mov %%ds, %0;" :"=r"(reg16) : /* no input */ );
    show_16_register( 'd', 's', 5, 24, reg16 );

//    asm( "mov %%cs, %0;" :"=r"(reg16) : /* no input */ );
//    show_16_register( 'c', 's', 4, 0, reg16 );

    int i = 0;
    int j = 10;
    int k = 50;

    reg16 = i + j + k;
    reg16 -= i;
    reg16 -= j;
//    reg16 -= k;
//    reg16 -= j;
//    reg16 -= i;
//    reg16 -= j;

    reg32 = get_eip();
    reg32 = get_eip();
    reg32 = get_eip();
//    reg32 = get_eip();
//    reg32 = get_eip();
//    reg32 = get_eip();
//    reg32 = get_eip();
//    reg32 = get_eip();
//    reg32 = get_eip();
//    reg32 = get_eip();
//    reg32 = get_eip();

//    show_16_register( 'd', 's', 6, 0, reg16 );
//    show_32_register( 'e', 'i', 'p', 6, 0, reg32 );

    for (;;) ;
}

void show_16_register( char a, char b, int row, int col, _U16 reg_value ) {
    _U8 high_nibble;
    _U8 low_nibble;

    _U16* memptr;

    memptr = (_U16 *)0xB8000;
    memptr += 80 * row + col;

    *memptr++ = 0x0f00 | a;
    *memptr++ = 0x0f00 | b;
    *memptr++ = 0x0f00 | ' ';
    *memptr++ = 0x0f00 | '=';
    *memptr++ = 0x0f00 | ' ';

    high_nibble = reg_value >> 4;
    low_nibble  = (_U8)(reg_value & 0x0f);

    *memptr++ = 0x0f00 | '0';
    *memptr++ = 0x0f00 | 'x';

    if (high_nibble < 10)
        *memptr++ = 0x0f00 | ('0' + high_nibble);
    else
        *memptr++ = 0x0f00 | ('a' - 10 + high_nibble);

    if (low_nibble < 10)
        *memptr++ = 0x0f00 | ('0' + low_nibble);
    else
        *memptr++ = 0x0f00 | ('a' - 10 + low_nibble);
}

void show_32_register( char a, char b, char c, int row, int col, _U32 reg_value ) {
//    _U16* memptr;
//
//    memptr = (_U16 *)0xB8000;
//    memptr += 80 * row + col;
//
//    *memptr++ = 0x0f00 | a;
//    *memptr++ = 0x0f00 | b;
//    *memptr++ = 0x0f00 | c;
//    *memptr++ = 0x0f00 | ' ';
//    *memptr++ = 0x0f00 | '=';
//    *memptr++ = 0x0f00 | ' ';
}
