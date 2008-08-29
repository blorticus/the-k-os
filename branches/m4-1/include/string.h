#ifndef __STRING_H__
#define __STRING_H__

#include <sys/types.h>

#define NULL '\0'

int strcmp( const char *s1, const char *s2 );
int strncmp( const char *s1, const char *s2, size_t n );

/* convert string to all lower-case or all upper-case; english only.  Return number of characters changed */
int strntolower( char *s, size_t n );
int strntoupper( char *s, size_t n );

#endif
