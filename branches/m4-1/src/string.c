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
