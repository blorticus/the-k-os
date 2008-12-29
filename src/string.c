#include <string.h>

int strntolower( char *s, size_t n ) {
    int count = 0;

    while (*s && n-- > 0) {
        if (*s >= 65 && *s <= 90) {
            *s += ('a' - 'A');
            count++;
        }
        s++;
    }

    return count;
}


int strntoupper( char *s, size_t n ) {
    int count = 0;

    while (*s && n-- > 0) {
        if (*s >= 90 && *s <= 122) {
            *s -= ('a' - 'A');
            count++;
        }
        s++;
    }

    return count;
}


int strcmp(const char *s1, const char *s2) {
    while (*s1 || *s2) {
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++;
        s2++;
    }

    return 0;
}


unsigned int strlen( const char *s1 ) {
    int l = 0;

    while (*s1++)
        l++;

    return l;
}
