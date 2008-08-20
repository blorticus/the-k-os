#ifndef __STRING_H__
#define __STRING_H__

#include <sys/types.h>

#define NULL '\0'

int strcmp( const char *s1, const char *s2 );
int strncmp( const char *s1, const char *s2, size_t n );

#endif
