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

    fbtt.WriteLineAt( U"The K-OS", { 1, 1 } );

    for ( ;; )
        ;
}

