//#include <cstdlib>
#include <string.h>
#include <boot/attributes.h>
#include <video/fb/cpp-text-terminal.h>
#include <video/SimpleFont.h>

void kmain( void ) {
    memaddr ba;
    boot_attributes bootattrs;
    FrameBuffer::TextTerminal fbtt;
    Mono8x16SimpleFont font;

    // need to make a localized copy, because the memory from the UEFI
    // bootloader will be trashed
    asm volatile( "movq %%r9, %0"
                  : "=r"(ba)
                  : 
                );

    memcpy( (void*)&bootattrs, (const void*)ba, sizeof(boot_attributes) );

    fbtt.setFrameBufferStartAddr( (uint32*)(bootattrs.fb_start_addr), bootattrs.fb_hrez, bootattrs.fb_vrez );
    fbtt.setActiveFont( &font );
    fbtt.setColors( FrameBuffer::White, FrameBuffer::Black );

    fbtt.clearScreen();

    fbtt.drawCharAt( 'T', 1, 0 );
    fbtt.drawCharAt( 'H', 1, 1 );
    fbtt.drawCharAt( 'E', 1, 2 );
    fbtt.drawCharAt( ' ', 1, 3 );
    fbtt.drawCharAt( 'K', 1, 4 );
    fbtt.drawCharAt( '-', 1, 5 );
    fbtt.drawCharAt( 'O', 1, 6 );
    fbtt.drawCharAt( 'S', 1, 7 );

//    fbtt_clear_screen( &fbtt );
//    fbtt_write_string( &fbtt, L"The K-OS\nBrought to you by\tNing Enterprises\n" );
//
//    fbtt_printf( &fbtt, L"- %d %d %d %d and %s\n", 10, 20, 30, 40, "a string" );

    for ( ;; )
        ;
}

