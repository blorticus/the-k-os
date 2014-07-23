#ifndef __STRING_H__
#define __STRING_H__

#include <sys/types.h>

void*  memmove ( void *dest, const void *src, size_t n );

void*  membset ( void *s, u8 c, size_t n );
void*  memdwset( void *s, u32 i, size_t n );

size_t strnlen ( const char *s, size_t maxlen );
char*  strncpy ( char *dest, const char *src, size_t n );



#endif
