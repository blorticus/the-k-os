#include <lcheck.h>
#include <video/kterm.h>
#include <sys/types.h>

/* cover functions that are unused by methods in this test but required as dependencies */
int upow( int a, int b ) { return 0; }
void ioport_readb( int port ) { return; }


// When TEST is defined, this is used for "putchar" like functions in b8000 textmode
#define SCREEN_WIDTH 6
#define SCREEN_HEIGHT 5
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

/* make the actual screen buffer a little larger than the "screen" matrix to verify
 * that operations don't write past the end of the "screen" */
#define SCREEN_BUFFER_SIZE SCREEN_SIZE + SCREEN_WIDTH

u16 screen[SCREEN_BUFFER_SIZE];

void TEST_b8000_write_char_at( u16 offset, u16 colors, char c ) {
    screen[offset] = ((u16)(colors << 8)) | (u16)c;
}

void* TEST_b8000_derive_screen_base_ptr( void ) {
    return screen;
}


/**
 * Compare the matrix of "screen" characters to the pattern string (which MUST be SCREEN_SIZE characters long).
 * If every character is the same, return 0.  If they differ, return the offset (starting at 1) of first character
 * that doesn't match.  It always verify that characters between SCREEN_BUFFER_SIZE and SCREEN_SIZE are NULL
 * (which is what happens in clear_screen_buffer()).  If the return value is greater than or equal to SCREEN_SIZE,
 * then the last routine wrote something past the "screen".
 **/
int cmp_screen_chars_to( const char* pattern ) {
    int i;
    char* c = (char*)pattern;

    for (i = 0; i < SCREEN_SIZE; i++, c++)
        if ((char)screen[i] != *c)
            return i + 1;

    for (i = SCREEN_SIZE; i < SCREEN_BUFFER_SIZE; i++)
        if (screen[i] != 0x0000)
            return i + 1;

    return 0;
}


/* wipe out the "screen" buffer, filling it with NULLs */
void clear_screen_buffer( void ) {
    int i;
    for (i = 0; i < SCREEN_BUFFER_SIZE; i++)
        screen[i] = 0x0000;
}


kterm_window rw;
KTERM_WINDOW root_window = &rw;

int main( void ) {
    lcheck_suite* s;
    int i;

    s = create_suite( "kterm" );

    clear_screen_buffer();

    kterm_init( SCREEN_HEIGHT, SCREEN_WIDTH );

    /* duplicate the root window struct */
    kterm_dup_root_window( root_window );

    /* verify members */
    fail_unless( s, root_window->height == SCREEN_HEIGHT,
                 "screen height is not SCREEN_HEIGHT" );
    fail_unless( s, root_window->width == SCREEN_WIDTH,
                 "screen width is not SCREEN_WIDTH" );
    fail_unless( s, root_window->base_offset == 0,
                 "base_offset is not 0" );
    fail_unless( s, root_window->current_offset == 0,
                 "current offset is not 0" );
    fail_unless( s, root_window->end_offset == SCREEN_HEIGHT * SCREEN_WIDTH - 1,
                 "end offset is not SCREEN_HEIGHT * SCREEN_WIDTH - 1" );

    /* cls() which should fill the "screen" with spaces */
    kterm_window_cls( root_window );

    fail_unless( s, cmp_screen_chars_to( "                              " ) == 0,
                 "kterm_window_cls() for root_window produced unexpected character" );

    /* write one character, then a second, then write until the last position.  Should not scroll at any point */
    clear_screen_buffer();
    kterm_dup_root_window( root_window );

    kterm_window_putc( root_window, 'A' );

    fail_unless( s, cmp_screen_chars_to( "A\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ) == 0,
                 "kterm_window_putc() single character comparison didn't succeed" );

    clear_screen_buffer();
    kterm_dup_root_window( root_window );

    kterm_window_putc( root_window, 'A' );
    kterm_window_putc( root_window, 'B' );

    fail_unless( s, cmp_screen_chars_to( "AB\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ) == 0,
                 "kterm_window_putc() two character comparison didn't succeed" );

    clear_screen_buffer();
    kterm_dup_root_window( root_window );

    for (i = 0; i < SCREEN_SIZE; i++)
        kterm_window_putc( root_window, 'A' + i );

    fail_unless( s, cmp_screen_chars_to( "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^" ) == 0,
                 "kterm_window_putc() single screen comparison didn't succeed" );

    /* now force scrolling with a single extra character */
    clear_screen_buffer();
    kterm_dup_root_window( root_window );

    for (i = 0; i < SCREEN_SIZE + 1; i++)
        kterm_window_putc( root_window, 'A' + i );

    fail_unless( s, cmp_screen_chars_to( "GHIJKLMNOPQRSTUVWXYZ[\\]^_     " ) == 0,
                 "kterm_window_putc() single screen comparison didn't succeed" );

    return conclude_suite( s );
}
