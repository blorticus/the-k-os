#include <string.h>
#include <boot/attributes.h>
#include <video/fb/TextTerminal.h>
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

    fbtt.SetFrameBufferStartAddr( (uint32*)(bootattrs.fb_start_addr), bootattrs.fb_hrez, bootattrs.fb_vrez );
    fbtt.SetActiveFont( &font );
    fbtt.SetColors( FrameBuffer::White, FrameBuffer::Black );

    fbtt.ClearScreen();

    fbtt.DrawCharAt( 'T', 1, 0 );
    fbtt.DrawCharAt( 'H', 1, 1 );
    fbtt.DrawCharAt( 'E', 1, 2 );
    fbtt.DrawCharAt( ' ', 1, 3 );
    fbtt.DrawCharAt( 'K', 1, 4 );
    fbtt.DrawCharAt( '-', 1, 5 );
    fbtt.DrawCharAt( 'O', 1, 6 );
    fbtt.DrawCharAt( 'Z', 1, 7 );

    for ( ;; )
        ;
}

