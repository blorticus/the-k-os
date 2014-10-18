#include <string.h>

void *memmove( void *dest, const void *src, size_t n ) {
    // number of quadwords in the size to copy
    size_t qwords   = n >> 3;
    size_t modbytes = n - (qwords << 3);

    asm volatile(
        "rep movsq"
        : 
        : "S"(src), "D"(dest), "c"(qwords)
    );

    if (modbytes) {
        asm volatile(
            "rep movsb"
            :
            : "S"((u64*)src + qwords), "D"((u64*)dest + qwords), "c"(modbytes)
        );
    }

    return dest;
}


void *membset( void *s, u8 c, size_t n ) {
    u64 extend;

    // number of quadwords in the size to copy
    size_t qwords   = n >> 3;
    size_t modbytes = n - (qwords << 3);

    if (qwords) {
        // extend byte eight times into quadword
        asm volatile(
            "rep stosb"
            :
            : "D"(&extend), "a"(c), "c"((u8)8)
        );

        asm volatile(
            "rep stosq"
            : 
            : "D"(s), "a"(extend), "c"(qwords)
        );
    }

    if (modbytes) {
        asm volatile(
            "rep stosb"
            :
            : "D"((u64*)s + qwords), "a"(c), "c"((u8)modbytes)
        );
    }

    return s;
}


void *memdwset( void *s, u32 i, size_t n ) {
    u64 extend;

    // number of quadwords in the size to copy
    size_t qwords    = n >> 1;
    size_t moddwords = n - (qwords << 1);

    if (qwords) {
        // extend double-word two times into quadword
        ((u32*)&extend)[0] = i;
        ((u32*)&extend)[1] = i;

        asm volatile(
            "rep stosq"
            :
            : "D"(s), "a"(extend), "c"(qwords)
        );
    }

    if (moddwords) {
        asm volatile(
            "rep stosl"
            :
            : "D"((u64*)s + qwords), "a"(i), "c"(moddwords)
        );
    }

    return s;
}


size_t strnlen( const char *s, size_t maxlen ) {
    size_t c = 0;
    char* t = (char*)s;

    while (*t++ && ++c < maxlen)
        ;

    return c;
}


char *strncpy( char *dest, const char *src, size_t n ) {
    asm volatile(
        "repnz movsb"
        :
        : "D"(dest), "S"(src), "c"(n)
    );

    return dest;
}


int strncmp( const char *s1, const char *s2, size_t n ) {
    while (n-- && *s1++ == *s2++)
        ;

    return *--s1 - *--s2;
}


int wcsncmp( const wchar_t* wcs1, const wchar_t* wcs2, size_t num ) {
    while (num-- && *wcs1++ == *wcs2++)
        ;

    return (*--wcs1 == *--wcs2 ? 0 : (*wcs1 > *wcs2 ? 1 : -1));
}

