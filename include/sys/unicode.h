#ifndef __UNICODE__
#define __UNICODE__

typedef struct {
    char c;
} unicode_char;

#define ascii_to_unicode_char(ascii_char) ((unicode_char){.c = ascii_char})
#define unicode_char_to_ascii(uchar) (uchar.c)

#endif
