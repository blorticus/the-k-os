#include <video/font.h>
#include <video/fb/text-terminal.h>
#include <sys/types.h>

void kmain( void ) {
    frame_buffer_text_terminal fbtt;
    int i, j;
    char c;

    fbtt_init( &fbtt, 1024, 768, 1, 2, (void*)0x80000000, 0xffffffff, 0x00000000 );

    fbtt_clear_screen( &fbtt );

    for (c = 'A', i = 0; c < 'A' + (768 / 16); c++, i++)
        for (j = 0; j < 15; j++)
            fbtt_draw_ascii_char_at( &fbtt, c, i, j );

    fbtt_scroll( &fbtt, 1 );

    for ( ;; )
        ;
}

