#include <video/b8000textmode.h>
#include <sys/types.h>
#include <stdio.h>

void kterm_create( void ) {
    textmode_default_init();     
    textmode_cls();
}


void kterm_puts( const char* str ) {
    textmode_puts( (char*)str );
}


void kterm_putc( char c ) {
    textmode_putc( c );
}


int kterm_putchar( int c ) {
    return textmode_putchar( c );
}


void kterm_puti( u32 i ) {
    textmode_puti( i );
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
    textmode_cls();
}

