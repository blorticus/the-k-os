#include <video/b8000textmode.h>
#include <sys/types.h>
#include <stdio.h>

u8 KTERM_SCREEN_HEIGHT;
u8 KTERM_SCREEN_WIDTH;

typedef struct kterm_window {
    B8000_TEXTMODE_WINDOW* tm_win;
} kterm_window;

typedef kterm_window* KTERM_WINDOW;

kterm_window rw;
KTERM_WINDOW root_window = &rw;

void kterm_init( u8 screen_height, u8 screen_width ) {
    KTERM_SCREEN_HEIGHT = screen_height;
    KTERM_SCREEN_WIDTH  = screen_width;

    init_textmode_window( root_window->tm_win, 1, screen_height, screen_width, make_b8000_colors( light_gray, black ) );
}

void kterm_create( void ) {
    kterm_init( 25, 80 );
    textmode_window_cls( root_window->tm_win );
}


void kterm_puts( const char* str ) {
    textmode_window_puts( root_window->tm_win, (char*)str );
}


void kterm_putc( char c ) {
    textmode_window_putc( root_window->tm_win, c );
}


int kterm_putchar( int c ) {
    return textmode_window_putchar( root_window->tm_win, c );
}


void kterm_puti( u32 i ) {
    textmode_window_puti( root_window->tm_win, i );
}


char hex_buf[11];  // 4 bytes (2 digits per byte) plus '0x' plus terminator
void kterm_puth( u32 i, u8 len ) {  // len is number of digits from left
    hex_buf[0] = '0';
    hex_buf[1] = 'x';
    hex_buf[10] = '\0';

    int last = 9;
    u8 nb;

    while (i) {
        nb = (u8)(i & 0x0f);
        if (nb < 10) {
            hex_buf[last--] = '0' + nb;
        }
        else {
            hex_buf[last--] = 'a' + (nb - 10);
        }
        i = i >> 4;
    }

    while (last > 1) {
        hex_buf[last--] = '0';
    }

    kterm_puts( (char*)hex_buf );
}


void kterm_readline( char* buffer, unsigned int size ) {
    char c;
    int  i = 0;

    while (i < size - 1) {
        c = (char)getchar();

        if (c == '\n') {
            kterm_putc( c );
            buffer[i] = '\0';
            return;
        }
        else if (c == '\t') {
            buffer[i++] = ' ';  /* convert tabs to single space */
            c = ' ';
        }
        else if (c == '\b') {
            if (i > 0) {
                i--;
            }
            else {
                continue;  // don't print backspace if we're at the start of the buffer
            }
        }
        else {
            buffer[i++] = c;
        }

        kterm_putc( c );
    }

    if (i == size - 1) {
        buffer[i] = '\0';
        kterm_putc( '\n' );
    }
}


void kterm_cls( void ) {
    textmode_window_cls( root_window->tm_win );
}

