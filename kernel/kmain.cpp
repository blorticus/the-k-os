#include <string.h>
//#include <boot/attributes.h>
#include <video/fb/TextTerminal.h>
#include <video/SimpleFont.h>
#include <bootboot.h>

/* these are set in the linker script */
extern BOOTBOOT bootboot;           // see bootboot.h
extern unsigned char *environment;  // configuration, UTF-8 text key=value pairs

void _start( void ) {
    FrameBuffer::TextTerminal fbtt;
    Mono8x16SimpleFont font;

    fbtt.SetFrameBufferStartAddr( (uint32*)bootboot.fb_ptr, bootboot.fb_width, bootboot.fb_height );
    fbtt.SetActiveFont( &font );
    fbtt.SetColors( FrameBuffer::White, FrameBuffer::Black );

    fbtt.ClearScreen();

    fbtt.WriteRuneString( U"The K-OS" );

    for ( ;; )
        ;
}

