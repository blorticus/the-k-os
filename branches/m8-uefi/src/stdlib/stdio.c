#include <stdio.h>
#include <sys/types.h>
//#include <input/keyboard.h>

//int getchar( void ) {
//    int c;
//    u16 scancodes;
//
//    /* grab this right off of the kernel queue.  Obviously, not the right way, but since there is only one process right now... */
//    for (;;)
//        if ((c = read_next_key_stroke( &scancodes )))
//            if ((char)c)    /* it is possible to just be a meta character, so we discard those */
//                return (int)((char)c);  /* lower half is the character */
//}


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



#define M_next_char(fp_arg_pos,rval,stack_pos,reg_vals)         \
    switch (int_arg_pos++) {                                    \
        case 0: rval = (char)reg_vals[0]; break;                \
        case 1: rval = (char)reg_vals[1]; break;                \
        case 2: rval = (char)reg_vals[2]; break;                \
        default: rval = *(char*)(stack_pos); stack_pos++;       \
    }


#define M_next_s32(int_arg_pos,rval,stack_pos,reg_vals)         \
    switch (int_arg_pos++) {                                    \
        case 0: rval = (s32)reg_vals[0]; break;                 \
        case 1: rval = (s32)reg_vals[1]; break;                 \
        case 2: rval = (s32)reg_vals[2]; break;                 \
        default: rval = *(s32*)(stack_pos); stack_pos++;        \
    }

#define M_next_s64(int_arg_pos,rval,stack_pos,reg_vals)         \
    switch (int_arg_pos++) {                                    \
        case 0: rval = (s64)reg_vals[0]; break;                 \
        case 1: rval = (s64)reg_vals[1]; break;                 \
        case 2: rval = (s64)reg_vals[2]; break;                 \
        default: rval = *(s64*)(stack_pos); stack_pos++;        \
    }


#define M_next_u32(int_arg_pos,rval,stack_pos,reg_vals)         \
    switch (int_arg_pos++) {                                    \
        case 0: rval = (u32)reg_vals[0]; break;                 \
        case 1: rval = (u32)reg_vals[1]; break;                 \
        case 2: rval = (u32)reg_vals[2]; break;                 \
        default: rval = *(u32*)(stack_pos); stack_pos++;        \
    }

#define M_next_u64(int_arg_pos,rval,stack_pos,reg_vals)         \
    switch (int_arg_pos++) {                                    \
        case 0: rval = (u64)reg_vals[0]; break;                 \
        case 1: rval = (u64)reg_vals[1]; break;                 \
        case 2: rval = (u64)reg_vals[2]; break;                 \
        default: rval = *(u64*)(stack_pos); stack_pos++;        \
    }


#define M_next_str(int_arg_pos,rval,stack_pos,reg_vals)         \
    switch (int_arg_pos++) {                                    \
        case 0: rval = (char*)reg_vals[0]; break;               \
        case 1: rval = (char*)reg_vals[1]; break;               \
        case 2: rval = (char*)reg_vals[2]; break;               \
        default: rval = *(stack_pos); stack_pos++;              \
    } 


#define M_next_float(fp_arg_pos,rval,reg_vals)                  \
    switch (fp_arg_pos++) {                                     \
        case 0: rval = (float)reg_vals[3]; break;               \
        case 1: rval = (float)reg_vals[4]; break;               \
        case 2: rval = (float)reg_vals[5]; break;               \
        case 3: rval = (float)reg_vals[6]; break;               \
        case 4: rval = (float)reg_vals[7]; break;               \
        case 5: rval = (float)reg_vals[8]; break;               \
        case 6: rval = (float)reg_vals[9]; break;               \
        case 7: rval = (float)reg_vals[10]; break;              \
        default: rval = *(float*)(stack_pos); stack_pos++;      \
    } 


#define M_produce_string(conv_func,base,tvar)   \
    s = buf;                                    \
    count = conv_func( buf, tvar, base, 19 );   \
    if (count > 0) {                            \
        padding -= count;                       \
        goto print_string;                      \
    }


char buf[20];
int cprintf( void (*putchar_f)(int, ...), char* putchar_args, const char *fmt, ... ) {
    unsigned int printed_chars = 0;
    char* p = (char*)fmt;

    u64 ulong = 0;
    s64 slong = 0;
    u32 uint  = 0;
    s32 sint  = 0;
    char c;
    unsigned int count;
    int padding;
    const char* s;

    int int_arg_pos = 0;    // count of int arguments already processed in varargs
//    int fp_arg_pos  = 0;    // count of floating point arguments already processed in varags

    u64 reg_vals[11];
    char** stack_offset;

    asm volatile( "movq %%rbp, %0" : "=r"(stack_offset) );
    stack_offset += 2;

    asm volatile( "movq %%rcx, %0"  : "=r"(reg_vals[0]) );
    asm volatile( "movq %%r8,  %0"  : "=r"(reg_vals[1]) );
    asm volatile( "movq %%r9,  %0"  : "=r"(reg_vals[2]) );

    asm volatile( "movq %%xmm0, %0" : "=r"(reg_vals[3]) );
    asm volatile( "movq %%xmm1, %0" : "=r"(reg_vals[4]) );
    asm volatile( "movq %%xmm2, %0" : "=r"(reg_vals[5]) );
    asm volatile( "movq %%xmm3, %0" : "=r"(reg_vals[6]) );
    asm volatile( "movq %%xmm4, %0" : "=r"(reg_vals[7]) );
    asm volatile( "movq %%xmm5, %0" : "=r"(reg_vals[8]) );
    asm volatile( "movq %%xmm6, %0" : "=r"(reg_vals[9]) );
    asm volatile( "movq %%xmm7, %0" : "=r"(reg_vals[10]) );

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

                case 'c':
                    M_next_char(int_arg_pos,c,stack_offset,reg_vals);
                    putchar_f( c, putchar_args );
                    printed_chars++;
                    break;

                case 'd':
                case 'i':
                    M_next_s32(int_arg_pos,sint,stack_offset,reg_vals);
                    M_produce_string(atoi,10,sint);

                    break;

                case 'l':
                    if (*(p + 1) == 'u') {
                        ++p;
                        M_next_u64(int_arg_pos,ulong,stack_offset,reg_vals);
                        M_produce_string(atoi,10,ulong);
                    }
                    else if (*(p + 1) == 'x') {
                        ++p;
                        M_next_s64(int_arg_pos,slong,stack_offset,reg_vals);
                        M_produce_string(atoi,16,slong);
                    }
                    else {
                        M_next_s64(int_arg_pos,slong,stack_offset,reg_vals);
                        M_produce_string(atoi,10,slong);
                    }

                    break;

                case 's':
                    M_next_str(int_arg_pos,s,stack_offset,reg_vals);

                    print_string:
                        while (padding-- > 0) {
                            putchar_f( ' ', putchar_args );
                            printed_chars++;
                        }

                        while (*s) {
                            putchar_f( *s++, putchar_args );
                            printed_chars++;
                        }

                    padding = 0;
                    break;

                case 'x':
                    M_next_s32(int_arg_pos,uint,stack_offset,reg_vals);
                    M_produce_string(atoi,16,uint);

                    break;

                case 'u':
                    M_next_u32(int_arg_pos,uint,stack_offset,reg_vals);
                    M_produce_string(atoi,10,uint);

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
