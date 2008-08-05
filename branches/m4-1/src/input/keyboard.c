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
    'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',    '\'',    0,    /*LS*/0,  0,     'z',    'x',    'c',    'v',    /* 2n */
    'b',    'n',    'm',    ',',    '.',    '/',   /*RS*/0,  0,    /*LA*/0, ' ',     0,      0,      0,      0,      0,      0,     /* 3n */
};


/* interrupt handler for keyboard activity.  Read scancode and print hex representation of the byte received on ioport 60. */
void keyboard_handler( struct regs *r ) {
    _U8 scancode;
    scancode = ioport_readb( 0x60 );

    /* if the high order bit is 1, this is the release of a key press */
    if ( !(scancode & 0x80) ) {
        if (us101_scancodes[scancode]) {
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
