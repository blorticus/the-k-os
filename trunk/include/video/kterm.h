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

void kterm_create_window( KTERM_WINDOW w, u16 window_start, u8 window_height, u8 window_width );
void kterm_init( u8 screen_height, u8 screen_width );
void kterm_dup_root_window( KTERM_WINDOW w );
KTERM_WINDOW kterm_get_root_window( void );
void kterm_window_cls( KTERM_WINDOW w );
void kterm_window_scroll( KTERM_WINDOW w );
void kterm_window_putc( KTERM_WINDOW w, char c );
void kterm_window_puts( KTERM_WINDOW w, const char* s );
void kterm_puts( const char* str );
void kterm_putc( char c );
int kterm_putchar( int c );
void kterm_window_puti( KTERM_WINDOW w, unsigned int i );
void kterm_puti( u32 i );
void kterm_window_puth( KTERM_WINDOW w, u32 i );
void kterm_puth( u32 i );
void kterm_window_readline( KTERM_WINDOW w, char* buffer, unsigned int size );
void kterm_readline( char* buffer, unsigned int size );
void kterm_cls( void );
void kterm_window_putchar_pf( int c, char* params );

//#define kterm_printf(fmt, ...) cprintf((void*)kterm_putchar, fmt, ## __VA_ARGS__)
#define kterm_window_printf(w,fmt, ...) cprintf((void*)kterm_window_putchar_pf, (char*)w, fmt, ## __VA_ARGS__)
