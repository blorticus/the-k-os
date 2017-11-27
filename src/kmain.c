#include <video/font.h>
#include <video/fb/text-terminal.h>
#include <sys/types.h>

void kmain( void ) {
    frame_buffer_text_terminal fbtt;
    int i;
    char c;

    fbtt_init( &fbtt, 1024, 768, 1, 2, (void*)0x80000000, 0xffffffff, 0x00000000 );

    for (c = 'A', i = 0; c <= 'Z'; c++, i++)
        fbtt_draw_ascii_char_at( &fbtt, c, i, i );

    for ( ;; )
        ;
}

