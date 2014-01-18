#include <string.h>
#include <sys/types.h>

void*
memset(void *b, int c, size_t len) {
    asm volatile( "cld; rep stosb" 
                   : 
                   : "c"(len), "a"(c), "D"(b)
                   : "memory", "flags" );
    return b;
}

