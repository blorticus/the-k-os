#ifndef __KTERM_H__
#define __KTERM_H__

void kterm_create( void );
void kterm_puts( const char* str );
void kterm_cls( void );
void kterm_putc( char c );
void kterm_readline( char* buffer, unsigned int size );


#endif
