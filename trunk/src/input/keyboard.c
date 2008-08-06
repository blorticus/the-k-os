#include <input/keyboard.h>
#include <idt.h>
#include <irq.h>
#include <sys/types.h>
#include <video/b8000textmode.h>
#include <sys/asm.h>


char us101_scancodes[] = {
/*   0       1       2       3       4       5       6       7       8       9       a       b       c       d       e       f      */
     0,      0,     '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    '-',    '=',    '\b',   '\t',   /* 0n */
    'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',    'o',    'p',    '[',    ']',    '\n',  /*LC*/0, 'a',    's',    /* 1n */
    'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',    '\'',   '`',   /*LS*/0, '\\',   'z',    'x',    'c',    'v',    /* 2n */
    'b',    'n',    'm',    ',',    '.',    '/',   /*RS*/0,  0,    /*LA*/0, ' ',     0,      0,      0,      0,      0,      0,     /* 3n */
};

char shifted_us101_scancodes[] = {
/*   0       1       2       3       4       5       6       7       8       9       a       b       c       d       e       f      */
     0,      0,     '!',    '@',    '#',    '$',    '%',    '^',    '&',    '*',    '(',    ')',    '_',    '+',    '\b',   '\t',   /* 0n */
    'Q',    'W',    'E',    'R',    'T',    'Y',    'U',    'I',    'O',    'P',    '{',    '}',    '\n',  /*LC*/0, 'A',    'S',    /* 1n */
    'D',    'F',    'G',    'H',    'J',    'K',    'L',    ':',    '"',    '~',   /*LS*/0, '|',    'Z',    'X',    'C',    'V',    /* 2n */
    'B',    'N',    'M',    '<',    '>',    '?',   /*RS*/0,  0,    /*LA*/0, ' ',     0,      0,      0,      0,      0,      0,     /* 3n */
};


_U8 is_shifted = 0;

/* interrupt handler for keyboard activity.  Read scancode and print hex representation of the byte received on ioport 60. */
void keyboard_handler( struct regs *r ) {
    _U8 scancode;
    scancode = ioport_readb( 0x60 );

    /* if this is a left or right shift key, then we are in shifted mode */
    if (scancode == 0x2a || scancode == 0x36) {
        is_shifted = 1;
    }
    /* otherwise, if this is the release of the left or right shift key, then we are unshifting (the high order bit is set to 1 if the key is being released) */
    else if ((scancode == (0x2a | 0x80)) || (scancode == (0x36 | 0x80))) {   /* let up shift key */
        is_shifted = 0;
    }
    /* otherwise, if this is not the release of a key ... */
    else if ( !(scancode & 0x80) ) {
        /* If we are in shifted mode, see if we know about the shifted key.  If so, print it in text mode.
         * If we are not in shifted mode, see if we know about the unshifted key, and if so, print it in text mode.
         * Whether in shifted mode or not, if we don't know about the key, print the hex value of the scancode */
        if (us101_scancodes[scancode]) {
            if (is_shifted)
                if (shifted_us101_scancodes[scancode])
                    textmode_putc( shifted_us101_scancodes[scancode] );
                else
                    textmode_put_hexbyte( scancode );
            else
                textmode_putc( us101_scancodes[scancode] );
        }
        else {
            textmode_put_hexbyte( scancode );
        }
    }
}

void keyboard_irq_install() {
    irq_install_handler( 1, keyboard_handler );
}
