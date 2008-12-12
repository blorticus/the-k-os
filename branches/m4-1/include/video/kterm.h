#ifndef __KTERM_H__
#define __KTERM_H__

void kterm_create( void );
void kterm_puts( const char* str );
void kterm_puti( u32 i );
void kterm_cls( void );
void kterm_putc( char c );

/* puts a 32-bit integer as hex text.  Output starts with 0x and always has 8 hex digits, even
 * if it must fill from the left with zeroes */
void kterm_puth( u32 i );

void kterm_readline( char* buffer, unsigned int size );


#endif
