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


size_t strlen( const char *s1 ) {
    int l = 0;

    while (*s1++)
        l++;

    return l;
}


int isstringint( const char* s ) {
    char* t = (char*)s;

    if (s == NULL || *t == '\0')
        return 0;

    if (*t == '-'|| *t == '+')
        t++;

    while (*t) {
        if (*t < '0' || *t > '9')
            return 0;
        t++;
    }

    return 1;
}


long
strtol( const char* str, char** endptr, int base ) {
    char* t = (char*)str;
    int is_negative = 0;
    long retval = 0;
    unsigned short digit;

    if (str == NULL || str[0] == '\0') {
        endptr = NULL;
        return 0;
    }

    if (*t == '-') {
        is_negative = 1;
        t++;
    }
    else if (*t == '+') {
        t++;
    }

    if (base > 36 || base < 2)
        base = 0;

    if (*t == '0') {
        if (t[1] == 'x') {
            base = 16;
            t += 2;
        }
        else if (t[1] == '\0') {
            if (endptr != NULL)
                *endptr = &t[1];
            return 0;
        }
        else {
            base = 8;
            t++;
        }
    }

    if (base == 0) {
        base = 10;
    }

    while (*t) {
        if (*t >= '0' && *t <= '9')
            digit = *t - '0';
        else if (*t >= 'a' && *t <= 'z')
            digit = 10 + *t - 'a';
        else if (*t >= 'A' && *t <= 'Z')
            digit = 10 + *t - 'A';
        else {  // invalid digit
            if (endptr != NULL)
                *endptr = t;
            return (is_negative ? retval * -1 : retval);
        }

        if (digit >= base) {
            if (endptr != NULL)
                *endptr = t;
            return (is_negative ? retval * -1 : retval);
        }

        retval = retval * base + digit;
        t++;
    }

    if (endptr != NULL)
        *endptr = t;

    return (is_negative ? retval * -1 : retval);
}
