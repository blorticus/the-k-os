#include <stdio.h>
#include <string.h>


/* Given a particular number base (currently 10 and 16 are supported), convert 'value' into its ascii numeric representation.
 * Place the acsii representation in 'buf', which will be null terminated.  If 'max_len' greater than characters needed to
 * fulfill request (not including the training null), the state of 'buf' will be undefined and the return value will be 0.
 * Returns length of string in 'buf' */
static unsigned int atoi( const char* buf, long value, unsigned int base, unsigned short max_len ) {
    int i = 0;
    unsigned char v;
    char* sbuf = (char*)buf;
    unsigned int count = 0;

    /* fill buffer with required characters in reverse order, then pad out required number of spaces.  Finally, reverse
     * the buffer */
    if (base == 16) {
        /* In this case, mask all but the lowest order 4 bits, extract its value, then shift 4 bits to the right.  Have to handle
         * value of 0 in a special way, because we ignore leading zeroes */
        do {
            if (max_len-- == 0)
                return 0;

            v = value & 0x0f;
            sbuf[i++] = v < 10 ? '0' + v : 'a' + (v - 10);
            count++;
            /* this must be cast to force the bit-shift operator to fill with zeros from the left */
            value = (unsigned long)value >> 4;
        } while (value);

        sbuf[i] = '\0';
    }
    else {  // ASSERT: base must equal 10
        /* In this case, mod by 10, then integer divide by 10 until value is zero.  This means we're working
         * backwards through this, so we'll have to reverse the buffer when we're done.  Also, we may need to
         * add a sign */
        if (value < 0) {
            if (--max_len < 1)
                return 0;

            sbuf[i++] = '-';
            count++;
            value *= -1;  // silly way to make absolute value, and inline asm would be faster, but...
        }

        do {
            if (max_len-- == 0)
                return 0;

            sbuf[i++] = '0' + (value % 10);
            count++;
            value /= 10;
        } while (value);

        sbuf[i] = '\0';
    }

    int j = sbuf[0] == '-' ? 1 : 0;  // skip negative sign if there is one

    char t;
    for( i-- ; j < i ; j++, i--) {  // i-- because i points at \0
        t = sbuf[j];
        sbuf[j] = sbuf[i];
        sbuf[i] = t;
    }

    return count;
}


static unsigned int wctoi( const char_t* buf, long value, unsigned int base, unsigned short max_len ) {
    int i = 0;
    unsigned char v;
    char_t* sbuf = (char_t*)buf;
    unsigned int count = 0;

    /* fill buffer with required characters in reverse order, then pad out required number of spaces.  Finally, reverse
     * the buffer */
    if (base == 16) {
        /* In this case, mask all but the lowest order 4 bits, extract its value, then shift 4 bits to the right.  Have to handle
         * value of 0 in a special way, because we ignore leading zeroes */
        do {
            if (max_len-- == 0)
                return 0;

            v = value & 0x0f;
            sbuf[i++] = v < 10 ? '0' + v : 'a' + (v - 10);
            count++;

            /* this must be cast to force the bit-shift operator to fill with zeros from the left */
            value = (unsigned long)value >> 4;
        } while (value);

        sbuf[i] = '\0';
    }
    else {  // ASSERT: base must equal 10
        /* In this case, mod by 10, then integer divide by 10 until value is zero.  This means we're working
         * backwards through this, so we'll have to reverse the buffer when we're done.  Also, we may need to
         * add a sign */
        if (value < 0) {
            if (--max_len < 1)
                return 0;

            sbuf[i++] = '-';
            count++;
            value *= -1;  // silly way to make absolute value, and inline asm would be faster, but...
        }

        do {
            if (max_len-- == 0)
                return 0;

            sbuf[i++] = '0' + (value % 10);
            count++;
            value /= 10;
        } while (value);

        sbuf[i] = '\0';
    }

    int j = sbuf[0] == '-' ? 1 : 0;  // skip negative sign if there is one

    char_t t;
    for( i-- ; j < i ; j++, i--) {  // i-- because i points at \0
        t = sbuf[j];
        sbuf[j] = sbuf[i];
        sbuf[i] = t;
    }

    return count;
}


char buf[20];
int cprintf( void (*putchar_f)(int, ...), char* putchar_args, const char *fmt, ... ) {
    const char** next_vararg = &fmt + 1;

    unsigned int printed_chars = 0;

    const char* p = fmt;

    long l;
    char c;
    unsigned int count;
    int padding;
    const char* s;

    while (*p) {
        padding = 0;
        if (*p != '%') {
            putchar_f( *p++, putchar_args );
            printed_chars++;
        }
        else {
            inner:
            switch (*++p) {
                case '%':
                    putchar_f( '%', putchar_args );
                    printed_chars++;
                    break;

                case 'i':
                case 'd':
                    l = (long)*next_vararg;
                    next_vararg++;
                    s = buf;
                    count = atoi( buf, l, 10, 19 );

                    if (count > 0) {
                        padding -= count; 
                        goto print_string;
                    }

                    break;

                case 'x':
                    l = (long)*next_vararg;
                    next_vararg++;
                    s = buf;
                    count = atoi( buf, l, 16, 19 );

                    if (count > 0) {
                        padding -= count;
                        goto print_string;
                    }

                    break;

                case 'c':
                    c = *((char*)next_vararg);
                    next_vararg++;
                    putchar_f( c, putchar_args );
                    printed_chars++;
                    break;

                case 's':
                    s = *next_vararg;
                    next_vararg++;

                    print_string:
                        while (padding-- > 0) {
                            putchar_f( ' ', putchar_args );
                            printed_chars++;
                        }

                        while (*s) {
                            putchar_f( *s++, putchar_args );
                            printed_chars++;
                        }

                    break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    padding = padding * 10 + (*p - '0'); 
                    goto inner;
            }

            p++;  // advance beyond format char
        }
    }

    return printed_chars;
}


typedef struct {
    uint64 first_non_fp;
    uint64 second_non_fp;
    uint64 third_non_fp;
    uint64 fourth_non_fp;
    uint64 fifth_non_fp;
    uint64 sixth_non_fp;

    double first_fp;
    double second_fp;
    double third_fp;
    double fourth_fp;
    double fifth_fp;
    double sixth_fp;
    double seventh_fp;
    double eighth_fp;
} t_saved_varargs;

static __attribute__((always_inline)) inline uint64 vargs_get_next_non_fp_arg( t_saved_varargs* va, int int_va_count ) {
    switch (int_va_count) {
        case 1: return va->first_non_fp; break;
        case 2: return va->second_non_fp; break;
        case 3: return va->third_non_fp; break;
        case 4: return va->fourth_non_fp; break;
        case 5: return va->fifth_non_fp; break;
        case 6: return va->sixth_non_fp; break;
        default:
            return 0;
            break;
    }
}

static __attribute__((always_inline)) inline uint64 vargs_get_next_fp_arg( t_saved_varargs* va, int fp_va_count ) {
    switch (fp_va_count) {
        case 1: return va->first_fp; break;
        case 2: return va->second_fp; break;
        case 3: return va->third_fp; break;
        case 4: return va->fourth_fp; break;
        case 5: return va->fifth_fp; break;
        case 6: return va->sixth_fp; break;
        case 7: return va->seventh_fp; break;
        case 8: return va->eighth_fp; break;
        default:
            return 0.0;
            break;
    }
}

int lprintf( void (*putwchar_f)(void*, int), void* putwchar_f_arg, const char_t *fmt, ... ) {
    char_t wcbuf[20];

    uint64 rax;
    t_saved_varargs va;

    asm __volatile__ ("movq %%rcx, %0\n\t"
                      "movq %%r8, %1\n\t"
                      "movq %%r9, %2"
                      : "=r"(va.fourth_non_fp), "=r"(va.fifth_non_fp), "=r"(va.sixth_non_fp)
                      :
                      );

    asm __volatile__ ("movq %%rax, %0" : "=r"(rax) : );

    if (rax > 0) {
        asm __volatile__ ("movq %%xmm0, %0\n\t"
                          "movq %%xmm1, %1\n\t"
                          "movq %%xmm2, %2\n\t"
                          "movq %%xmm3, %3\n\t"
                          "movq %%xmm4, %4\n\t"
                          "movq %%xmm5, %5\n\t"
                          "movq %%xmm6, %6\n\t"
                          "movq %%xmm7, %7"
                          : "=r"(va.first_fp), "=r"(va.second_fp), "=r"(va.third_fp), "=r"(va.fourth_fp),
                            "=r"(va.fifth_fp), "=r"(va.sixth_fp), "=r"(va.seventh_fp), "=r"(va.eighth_fp)
                          :
                         );
    }

    unsigned int printed_chars = 0;

    const char_t* p = fmt;

    int next_non_fp_arg_num = 4;

    long l;
    char c;
    unsigned int count;
    int padding;
    const char* s;
    const char_t* w;

    while (*p) {
        padding = 0;
        if (*p != '%') {
            putwchar_f( putwchar_f_arg, *p++ );
            printed_chars++;
        }
        else {
            inner:
            switch (*++p) {
                case '%':
                    putwchar_f( putwchar_f_arg, '%' );
                    printed_chars++;
                    break;

                case 'i':
                case 'd':
                    //l = (int)*next_stack_arg++;
                    l = vargs_get_next_non_fp_arg( &va, next_non_fp_arg_num++ );
                    w = wcbuf;
                    count = wctoi( wcbuf, l, 10, 19 );

                    if (count > 0) {
                        padding -= count; 
                        goto print_wchar_string;
                    }

                    break;

                case 'x':
                    //l = (long)*next_stack_arg++;
                    l = vargs_get_next_non_fp_arg( &va, next_non_fp_arg_num++ );
                    w = wcbuf;
                    count = wctoi( wcbuf, l, 16, 19 );

                    if (count > 0) {
                        padding -= count;
                        goto print_wchar_string;
                    }

                    break;

                case 'c':
                    //c = *((char*)next_vararg);
                    c = (char)(vargs_get_next_non_fp_arg( &va, next_non_fp_arg_num++ ));
                    putwchar_f( putwchar_f_arg, c );
                    printed_chars++;
                    break;

                case 's':
                    s = (char*)(vargs_get_next_non_fp_arg( &va, next_non_fp_arg_num++ ));

                    while (padding-- > 0) {
                        putwchar_f( putwchar_f_arg, ' ' );
                        printed_chars++;
                    }

                    while (*s) {
                        putwchar_f( putwchar_f_arg, *s++ );
                        printed_chars++;
                    }

                    break;

                case 'w':
                    w = (char_t*)(vargs_get_next_non_fp_arg( &va, next_non_fp_arg_num++ ));

                    print_wchar_string:
                         while (padding-- > 0) {
                            putwchar_f( putwchar_f_arg, ' ' );
                            printed_chars++;
                        }

                        while (*w) {
                            putwchar_f( putwchar_f_arg, *w++ );
                            printed_chars++;
                        }

                    break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    padding = padding * 10 + (*p - '0'); 
                    goto inner;
            }

            p++;  // advance beyond format char
        }
    }

    return printed_chars;
}
