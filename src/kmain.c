#include <video/font.h>
#include <video/fb/text-terminal.h>
#include <sys/types.h>

void kmain( void ) {
    frame_buffer_text_terminal fbtt;
//    int i, j;
//    char c;

    fbtt_init( &fbtt, 1024, 768, 1, 2, (void*)0x80000000, 0xffffffff, 0x00000000 );

    fbtt_clear_screen( &fbtt );
    fbtt_write_string( &fbtt, L"The K-OS\nBrought to you by\tNing Enterprises" );

    for ( ;; )
        ;
}

