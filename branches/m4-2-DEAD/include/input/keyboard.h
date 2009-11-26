#ifndef __INPUT_KEYBOARD_H__
#define __INPUT_KEYBOARD_H__

#include <sys/types.h>

//char us101_scancodes[] = {
///*   0       1       2       3       4       5       6       7       8       9       a       b       c       d       e       f      */
//     0,      0,     '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    '-',    '=',    '\b',   '\t',   /* 0n */
//    'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',    'o',    'p',    '[',    ']',    '\n',  /*LC*/0, 'a',    's',    /* 1n */
//    'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',    '\'',   '`',   /*LS*/0, '\\',   'z',    'x',    'c',    'v',    /* 2n */
//    'b',    'n',    'm',    ',',    '.',    '/',   /*RS*/0,  0,    /*LA*/0, ' ',     0,      0,      0,      0,      0,      0,     /* 3n */
//};
//
//char shifted_us101_scancodes[] = {
///*   0       1       2       3       4       5       6       7       8       9       a       b       c       d       e       f      */
//     0,      0,     '!',    '@',    '#',    '$',    '%',    '^',    '&',    '*',    '(',    ')',    '_',    '+',    '\b',   '\t',   /* 0n */
//    'Q',    'W',    'E',    'R',    'T',    'Y',    'U',    'I',    'O',    'P',    '{',    '}',    '\n',  /*LC*/0, 'A',    'S',    /* 1n */
//    'D',    'F',    'G',    'H',    'J',    'K',    'L',    ':',    '"',    '~',   /*LS*/0, '|',    'Z',    'X',    'C',    'V',    /* 2n */
//    'B',    'N',    'M',    '<',    '>',    '?',   /*RS*/0,  0,    /*LA*/0, ' ',     0,      0,      0,      0,      0,      0,     /* 3n */
//};


/* so-called ancillary keys that might be depressed at the same time as a character key */
#define CAPS_LOCK       0x01
#define LEFT_SHIFT      0x02
#define LEFT_CTRL       0x04
#define LEFT_META       0x08
#define RIGHT_META      0x10
#define RIGHT_CTRL      0x20
#define RIGHT_SHIFT     0x40


/* Scancode circular queue struct */
typedef struct {
    u16*            slots;
    u16             size;       /* number of slots */
    u16             adjust;     /* MAX(top) % size, or the new offset for top when it wraps to 0 */
    s16             top;        /* index of last character written to queue */
    s16             bottom;     /* index of last character read from queue */
} ScCQueue;


u16 read_next_key_stroke( u16* scancodes );
void keyboard_irq_install();


#endif
