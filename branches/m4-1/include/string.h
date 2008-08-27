#ifndef __STRING_H__
#define __STRING_H__

#include <sys/types.h>

#define NULL '\0'

int strcmp( const char *s1, const char *s2 );

#if defined (TESTING)
int strncmp( const char *s1, const char *s2, _size_t n );
#else
int strncmp( const char *s1, const char *s2, size_t n );
#endif

#endif
