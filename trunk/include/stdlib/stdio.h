#ifndef __STDIO_H__
#define __STDIO_H__

int getchar( void );
int cprintf( void (*putchar_f)(int, ...), char* putchar_args, const char *fmt, ... );


#endif
