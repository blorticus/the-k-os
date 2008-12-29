#ifndef __KTERM_H__
#define __KTERM_H__

#include <stdio.h>
#include <sys/types.h>

void kterm_create( void );
void kterm_puts( const char* str );
void kterm_puti( u32 i );
void kterm_cls( void );
void kterm_putc( char c );

/* puts a 32-bit integer as hex text.  Output starts with 0x and always has 8 hex digits, even
 * if it must fill from the left with zeroes */
void kterm_puth( u32 i );

void kterm_readline( char* buffer, unsigned int size );

/* like libc putchar() */
int kterm_putchar( int c );

#define kterm_printf(fmt, ...) cprintf((void*)kterm_putchar, fmt, ## __VA_ARGS__)


#endif
