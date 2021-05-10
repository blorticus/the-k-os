#include <stdio.h>
#include <stdint.h>

void main(void) {
    uint32_t a = 0x11223344;

    uint8_t* b = (uint8_t*)&a;

    for (int i = 0; i < 4; i++)
        printf( "0x%02x ", b[i] );

    printf( "\n" );

    printf( "%08x\n", a );
}
