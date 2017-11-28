#include <string.h>
#include <sys/types.h>

void*
memset(void *b, int c, size_t len) {
    // The storage of 'b' in a register, then returning to 'b' after this
    // operation is completed ensures that we return the original value of 'b'.
    // If we attempt this by assignment, the optimizer will likely remove it
    asm volatile( "cld; rep stosb" 
                   : "=b"(b)
                   : "c"(len), "a"(c), "b"(b), "D"(b)
                   : "memory", "flags" );

    return b;
}

void*
memcpy(void *dest, const void* src, size_t n) {
    asm volatile( "cld; rep movsb"
                    : "=b"(dest)
                    : "c"(n), "b"(dest), "S"(src), "D"(dest)
                    : "memory", "flags" );

    return dest;
}
