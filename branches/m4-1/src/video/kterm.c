#include <video/b8000textmode.h>
#include <sys/types.h>
#include <stdio.h>

void kterm_create( void ) {
    textmode_default_init();     
    textmode_cls();
}


void kterm_puts( const char* str ) {
    textmode_puts( str );
}


void kterm_putc( char c ) {
    textmode_putc( c );
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

