#include <video/b8000textmode.h>
#include <sys/types.h>
#include <stdio.h>


typedef struct kterm_window {
    u8 width;
    u8 height;
    u16 base_offset;
    u16 end_offset;
    u16 current_offset;
    u8 colors;
} kterm_window;

typedef kterm_window* KTERM_WINDOW;
static kterm_window rw;
static KTERM_WINDOW root_window = &rw;

void kterm_create_window( KTERM_WINDOW w, u16 window_start, u8 window_height, u8 window_width ) {
    w->width = window_width;
    w->height = window_height;
    w->base_offset = window_start;
    w->end_offset  = window_start + window_width * window_height - 1;
    w->current_offset = window_start;
    w->colors = make_b8000_colors( light_gray, black );
}

void kterm_init( u8 screen_height, u8 screen_width ) {
    kterm_create_window( root_window, 0, screen_height, screen_width );
}

KTERM_WINDOW kterm_get_root_window( void ) {
    return root_window;
}

void kterm_dup_root_window( KTERM_WINDOW w ) {
    u32* d = (u32*)w;
    u32* s = (u32*)root_window;

    *d++ = *s++;
    *d = *s;
    w->colors = root_window->colors;
}


void kterm_window_cls( KTERM_WINDOW w ) {
    int i;
    for (i = w->base_offset; i <= w->end_offset; i++)
        M_B8000_write_char_at( i, w->colors, ' ' );
    w->current_offset = w->base_offset;
}

void kterm_window_scroll( KTERM_WINDOW w ) {
    textmode_copy_back( w->base_offset + w->width, w->end_offset, w->width );

    w->current_offset = w->end_offset - w->width + 1;

    while (w->current_offset++ < w->end_offset)
        M_B8000_write_char_at( w->current_offset, w->colors, ' ' );

    w->current_offset = w->end_offset - w->width + 1;
}


void kterm_window_putc( KTERM_WINDOW w, char c ) {
    int i;

    if (c == '\r') {
        w->current_offset -= (w->current_offset - w->base_offset) % w->width;
    }
    else if (c == '\n') {
        w->current_offset += (w->width - ((w->current_offset - w->base_offset) % w->width));
    }
    else if (c == '\t') {
        for (i = 0; i < 4; i++)
            M_B8000_write_char_at( w->current_offset++, w->colors, ' ' );
    }
    else if (c == '\b') {
        if (w->current_offset > w->base_offset) {
            w->current_offset--;
            M_B8000_write_char_at( w->current_offset, w->colors, ' ' );
        }
    }
    else if (c >= ' ') {
        if (w->current_offset > w->end_offset) {
            kterm_window_scroll( w );
            w->current_offset = w->end_offset - w->width + 1;
        }
        M_B8000_write_char_at( w->current_offset, w->colors, c );
        w->current_offset++;
    }
}


void kterm_window_puts( KTERM_WINDOW w, const char* s ) {
    char* t = (char*)s;
    while (*t)
        kterm_window_putc( w, *t++ );
}


void kterm_puts( const char* str ) {
//    textmode_window_puts( root_window->tm_win, (char*)str );
    kterm_window_puts( root_window, str );
}


void kterm_putc( char c ) {
//    textmode_window_putc( root_window->tm_win, c );
    kterm_window_putc( root_window, c );
}


int kterm_window_putchar( KTERM_WINDOW w, int c ) {
    kterm_window_putc( w, (unsigned char)c );
    return c;
}


int kterm_putchar( int c ) {
    kterm_window_putc( root_window, (unsigned char)c );
    return c;
}


/* static buffer for the decimal string, and a pointer to the first element in 'c' where a valid digit exists, since we'll build this backwards */
char int_buff[11];

void kterm_window_puti( KTERM_WINDOW w, unsigned int i ) {
    int_buff[10] = '\0';

    u8 last_digit = 10;

    if (i == 0)
        int_buff[--last_digit] = '0';
    else
        while (i) {
            int_buff[--last_digit] = '0' + (i % 10);
            i = i / 10;
        }

    kterm_window_puts( w, (const char*)(int_buff + last_digit) );
}


void kterm_puti( u32 i ) {
    kterm_window_puti( root_window, i );
}


char hex_buf[11];  // 4 bytes (2 digits per byte) plus '0x' plus terminator
void kterm_window_puth( KTERM_WINDOW w, u32 i ) {
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

    kterm_window_puts( w, (char*)hex_buf );
}


void kterm_puth( u32 i ) {
    kterm_window_puth( root_window, i );
}

void kterm_window_readline( KTERM_WINDOW w, char* buffer, unsigned int size ) {
    char c;
    int  i = 0;

    while (i < size - 1) {
        c = (char)getchar();

        if (c == '\n') {
//            kterm_putc( c );
            kterm_window_putc( w, c );
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

//        kterm_putc( c );
        kterm_window_putc( w, c );
    }

    if (i == size - 1) {
        buffer[i] = '\0';
//        kterm_putc( '\n' );
        kterm_window_putc( w, '\n' );
    }
}

void kterm_readline( char* buffer, unsigned int size ) {
    kterm_window_readline( root_window, buffer, size );
}


void kterm_cls( void ) {
    kterm_window_cls( root_window );
}


void kterm_window_putchar_pf( int c, char* params ) {
    kterm_window_putc( (KTERM_WINDOW)params, (char)c );
}
