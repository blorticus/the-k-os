#include <stdio.h>

/* Given a particular number base (currently 10 and 16 are supported), convert 'value' into its ascii numeric representation.
 * Place the acsii representation in 'buf', which will be null terminated */
static void itoa( char* buf, long value, unsigned int base ) {
    int i = 0;
    unsigned char v;
    char* sbuf = buf;

    if (base == 16) {
        /* In this case, look at the high order bits, shifting them left until all leading nibbles of 0 are gone.
         * Then, continue shifting nibbles, converting them into their proper representation.  Handle the special
         * case where the value is zero */
        if (value == 0) {
            sbuf[i++] = '0'; sbuf[i] = '\0';
            return;
        }

        while (value) {
            v = value & 0x0f;
            sbuf[i++] = v < 10 ? '0' + v : 'a' + (v - 10);
            value = value >> 4;
        }

        sbuf[i--] = '\0';
    }
    else {  // ASSERT: base must equal 10
        /* In this case, mod by 10, then integer divide by 10 until value is zero.  This means we're working
         * backwards through this, so we'll have to reverse the buffer when we're done.  Also, we may need to
         * add a sign */
        if (value < 0) {
            sbuf[i++] = '-';
            value *= -1;  // silly way to make absolute value, and inline asm would be faster, but...
        }

        do {
            sbuf[i++] = '0' + (value % 10);
            value /= 10;
        } while (value);

        sbuf[i--] = '\0';
    }

    int j = sbuf[0] == '-' ? 1 : 0;

    char t;
    for( ; j < i; j++, i--) {
        t = sbuf[j];
        sbuf[j] = sbuf[i];
        sbuf[i] = t;
    }
}


char buf[20];
int cprintf( void (*putchar_f)(int), const char *fmt, ... ) {
    const char** next_vararg = &fmt + 1;

    unsigned int printed_chars = 0;

    const char* p = fmt;

    long l;
    char c;
    const char* s;

    while (*p) {
        if (*p != '%') {
            putchar_f( *p++ );
            printed_chars++;
        }
        else {
            switch (*++p) {
                case '%':
                    putchar_f( '%' );
                    printed_chars++;
                    break;

                case 'i':
                case 'd':
                    l = (long)*next_vararg;
                    next_vararg++;
                    s = buf;
                    itoa( buf, l, 10 );

                    goto print_string;

                    break;

                case 'x':
                    l = (long)*next_vararg;
                    next_vararg++;
                    s = buf;
                    itoa( buf, l, 16 );

                    goto print_string;

                    break;

                case 'c':
                    c = *((char*)next_vararg);
                    next_vararg++;
                    putchar_f( c );
                    printed_chars++;
                    break;

                case 's':
                    s = *next_vararg;
                    next_vararg++;

                    print_string:
                    while (*s) {
                        putchar_f( *s++ );
                        printed_chars++;
                    }

                    break;
                    
            }

            p++;  // advance beyond format char
        }
    }

    return printed_chars;
}
