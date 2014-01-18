#include <lcheck.h>
#include <video/kterm.h>
#include <string.h>


/* Two mock implementations to facilitate testing */

/* For getchar(), seed the next stream of characters via seed_getchar().  Each call to getchar() will
 * consume one character at a time.  Once \0 is reached, getchar() will return \0 repeatedly until
 * seed_getchar() is called again */

const char* G_getchar_stream = "";
void seed_getchar( const char* s ) {
    G_getchar_stream = s;
}

int getchar( void ) {
    if (*G_getchar_stream) {
        return *G_getchar_stream++;
    }
    else {
        return '\0';
    }
}

/* to see what was last inserted via kterm_window_putc(), set a buffer to receive the results via
 * set_putc_receiver(), giving it the buffer pointer and the buffer length.  kterm_window_putc()
 * will not write past length - 1.  The last character of the buffer will always a null terminator */
char*          G_putc_buf = 0;
unsigned int   G_putc_buf_len = 0;
unsigned int   G_putc_buf_lw  = 0;

void set_putc_receiver( char* buf, unsigned int buflen ) {
    G_putc_buf     = buf;
    G_putc_buf_len = buflen;
    G_putc_buf_lw  = 0;
    G_putc_buf[buflen-1] = '\0';
}

void putc_receiver_rewind( void ) {
    G_putc_buf_lw = 0;
    if (G_putc_buf && G_putc_buf_len)
        G_putc_buf[0] = '\0';
}

void testing_kterm_window_putc( KTERM_WINDOW w, char c ) {
    if (G_putc_buf && G_putc_buf_len) {
        if (G_putc_buf_lw < G_putc_buf_len - 2) {
            G_putc_buf[G_putc_buf_lw++] = c;
            G_putc_buf[G_putc_buf_lw] = '\0';
        }

        G_putc_buf[G_putc_buf_len - 1] = '\0';
    }
}

void TEST_b8000_write_char_at( u16 offset, u16 colors, char c ) {
}

void* TEST_b8000_derive_screen_base_ptr( void ) {
    return (void*)G_putc_buf;
}


kterm_window rw;
KTERM_WINDOW root_window = &rw;

int main( void ) {
    lcheck_suite* s;
    int i;

    s = create_suite( "kterm_window_readline" );

    /* these are just to get a properly formatted KTERM_WINDOW struct that we
     * can pass to kterm_window_* methods.   We won't use it for anything else here */
    kterm_init( 10, 10 );
    kterm_dup_root_window( root_window );

    char readline_buffer[100];
    for (i = 0; i < 100; i++)
        readline_buffer[i] = '\0';

    char getc_recv[100];
    for (i = 0; i < 100; i++)
        getc_recv[i] = '\0';

    set_putc_receiver( getc_recv, 100 );

    seed_getchar( "" );
    kterm_window_cls( root_window );  /* each cls() is needed to reset the kterm window pointer */
    kterm_window_readline( root_window, readline_buffer, 100 );
    fail_unless( s, strcmp( readline_buffer, "" ) == 0,
                 "kterm_window_readline() fails to set buffer to empty string when no characters provided" );
    fail_unless( s, strcmp( getc_recv, "" ) == 0,
                 "kterm_window_readline() puts something in putc buffer even when empty string provided" );

    putc_receiver_rewind();
    seed_getchar( "\n" );
    kterm_window_cls( root_window );
    putc_receiver_rewind();
    kterm_window_readline( root_window, readline_buffer, 100 );
    fail_unless( s, strcmp( readline_buffer, "" ) == 0,
                 "kterm_window_readline() fails to set buffer to empty string when only newline is provided" );
    fail_unless( s, strcmp( getc_recv, "\n" ) == 0,
                 "kterm_window_readline() failed to put only newline in getc buffer when only newline is provided" );

    putc_receiver_rewind();
    seed_getchar( "foo\n" );
    kterm_window_cls( root_window );
    putc_receiver_rewind();
    kterm_window_readline( root_window, readline_buffer, 100 );
    fail_unless( s, strcmp( readline_buffer, "foo" ) == 0,
                 "kterm_window_readline() fails to set buffer to 'foo' when string 'foo\\n' is provided" );
    fail_unless( s, strcmp( getc_recv, "foo\n" ) == 0,
                 "kterm_window_readline() puts fails to set putc buffer to 'foo\\n' when string 'foo\\n' is provided" );

    putc_receiver_rewind();
    seed_getchar( "This\n" );
    kterm_window_cls( root_window );
    putc_receiver_rewind();
    kterm_window_readline( root_window, readline_buffer, 100 );
    fail_unless( s, strcmp( readline_buffer, "This" ) == 0,
                 "kterm_window_readline() fails to set buffer to 'This' when string 'This\\n' is provided" );
    fail_unless( s, strcmp( getc_recv, "This\n" ) == 0,
                 "kterm_window_readline() puts fails to set putc buffer to 'This\\n' when string 'This\\n' is provided" );

    putc_receiver_rewind();
    seed_getchar( "This Is the!2 end Of tHE # wOrLd%\n" );
    kterm_window_cls( root_window );
    putc_receiver_rewind();
    kterm_window_readline( root_window, readline_buffer, 100 );
    fail_unless( s, strcmp( readline_buffer, "This Is the!2 end Of tHE # wOrLd%" ) == 0,
                 "kterm_window_readline() fails to set buffer to correct string when spaces and non-alpha chars used" );
    fail_unless( s, strcmp( getc_recv, "This Is the!2 end Of tHE # wOrLd%\n" ) == 0,
                 "kterm_window_readline() puts fails to set putc buffer to proper value when spaces and non-alpha chars used" );

    putc_receiver_rewind();
    seed_getchar( "This\b\b\b\bthis Is the!2 end Of tHE\b\b # wOrLd%\n" );
    kterm_window_cls( root_window );
    putc_receiver_rewind();
    kterm_window_readline( root_window, readline_buffer, 100 );
    fail_unless( s, strcmp( readline_buffer, "this Is the!2 end Of t # wOrLd%" ) == 0,
                 "kterm_window_readline() fails to set buffer to correct string when spaces and backspaces used" );
    fail_unless( s, strcmp( getc_recv, "This\b\b\b\bthis Is the!2 end Of tHE\b\b # wOrLd%\n" ) == 0,
                 "kterm_window_readline() puts fails to set putc buffer to proper value when spaces and backspaces used" );

    return conclude_suite( s );
}
