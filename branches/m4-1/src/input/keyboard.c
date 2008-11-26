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


///* so-called ancillary keys that might be depressed at the same time as a character key */
//

/* count of characters to enqueue from keyboard in circular buffer,  CANNOT BE LARGER THAN 2**16 - 1 */
#define KEYBOARD_QUEUE_SIZE     10240

//
///* Scancode circular queue struct */
//typedef struct {
//    u16*            slots;
//    u16             limit;      /* MUST BE < MAX(s16) - 1, this is the number of the last slot, and thus, is sizeof( slots ) - 1 */
//    s16             top;        /* index of last character written to queue */
//    s16             bottom;     /* index of last character read from queue */
//} ScCQueue;


/* The actual keyboard character queue */
ScCQueue keyboard_buffer_queue;

u16 kb_slots[KEYBOARD_QUEUE_SIZE];  /* without kmalloc() yet, pre-allocate keyboard buffer.  Later this should be done when IRQ is installed */


/* Interruptable?       YES
 * Re-entrant?          NO
 */
void keyboard_handler( struct regs *r ) {
    int scancode;

    scancode = ioport_readb( 0x60 );

    keyboard_buffer_queue.slots[keyboard_buffer_queue.top++] = scancode;

//    /* if the top is immediately before the bottom, then consider the queue full -- though it is one less than full */
//    /* the top pointer MUST BE moved last.  If this handler is interrupted by the reader, the reader will advance the bottom pointer.  The
//     * relationship between the two pointers determines whether the queue is full (well, often full - 1) */
//    if (((keyboard_buffer_queue.top + 1) % keyboard_buffer_queue.limit) != keyboard_buffer_queue.bottom) {   /* that is, the queue isn't full */
//        keyboard_buffer_queue.slots[keyboard_buffer_queue.top + 1] = scancode;
//        keyboard_buffer_queue.top = (keyboard_buffer_queue.top + 1) % keyboard_buffer_queue.limit;
//    }

}


/* Interruptable?       YES
 * Re-entrant?          NO
 *
 * Read the next scancode in the scancode buffer.  If it is a non-meta ("character") key being depressed,
 * set lower half of return value to the ASCII value for that key.  If it is a meta key (shift lock,
 * shift keys, control keys, or meta/alt keys) set upper half to the appropriate MetaKey type value.  If
 * a meta key is held down while a character key is pressed, both upper and lower will be set.  If the
 * meta key is a shift key, or the caps lock is down, character keys will be shifted ASCII value.  If the
 * scancode buffer is empty, return 0x0000
 */

#define CAPS_LOCK       0x01
#define LEFT_SHIFT      0x02
#define LEFT_CTRL       0x04
#define LEFT_META       0x08
#define RIGHT_META      0x10
#define RIGHT_CTRL      0x20
#define RIGHT_SHIFT     0x40

/* bit field indicating what meta key is currently depressed */
u8 G_meta_keys = 0x00;


u16 read_next_key_stroke( void ) {
    u16 scancode;

    char c = '\0';

//    int next_slot;
    if (keyboard_buffer_queue.bottom == keyboard_buffer_queue.top)    /* queue is empty */
        return 0x0000;


    /* ASSERT: keyboard_buffer_queue is not empty */
//    next_slot = (keyboard_buffer_queue.bottom + 1) % keyboard_buffer_queue.limit;
//    scancode = keyboard_buffer_queue.slots[next_slot];
//    keyboard_buffer_queue.bottom = next_slot;  /* this advance MUST happen after the scancode is dequeued */

    scancode = keyboard_buffer_queue.slots[keyboard_buffer_queue.bottom++];

    /* we now have the next scancode */
    switch (scancode) {
        case 0x2a:
            G_meta_keys |= LEFT_SHIFT;
            break;

        case 0x2a | 0x80:
            G_meta_keys &= (~LEFT_SHIFT);
            break;

        case 0x36:
            G_meta_keys |= RIGHT_SHIFT;
            break;

        case 0x36 | 0x80:
            G_meta_keys &= (~RIGHT_SHIFT);
            break;

        case 0x1d:
            G_meta_keys |= LEFT_CTRL;
            break;

        case 0x1d | 0x80:
            G_meta_keys &= (~LEFT_CTRL);
            break;

//        case XXX:
//            G_meta_keys |= RIGHT_CTRL;
//            break;

        case 0x38:
            G_meta_keys |= LEFT_META;
            break;

        case 0x38 | 0x80:
            G_meta_keys &= (~LEFT_META);
            break;

//        case XXX:
//            G_meta_keys |= RIGHT_ALT;
//            break;

        default:
            if ( !(scancode & 0x80) ) {  /* that is, key is not being released -- so it is being depressed */
                if ((G_meta_keys & LEFT_SHIFT) | (G_meta_keys & RIGHT_SHIFT)) {
                    if (shifted_us101_scancodes[scancode]) {
                        c = shifted_us101_scancodes[scancode];
                    }
                }
                else { /* not shifted */
                    if (us101_scancodes[scancode]) {
                        c = us101_scancodes[scancode];
                    }
                }
            }
    }

    return (((u16)G_meta_keys << 8) | (u16)c);
}


///* interrupt handler for keyboard activity.  Read scancode and print hex representation of the byte received on ioport 60. */
//void old_keyboard_handler( struct regs *r ) {
//    _U8 scancode;
//    scancode = ioport_readb( 0x60 );
//
//    /* if this is a left or right shift key, then we are in shifted mode */
//    if (scancode == 0x2a || scancode == 0x36) {
//        is_shifted = 1;
//    }
//    /* otherwise, if this is the release of the left or right shift key, then we are unshifting (the high order bit is set to 1 if the key is being released) */
//    else if ((scancode == (0x2a | 0x80)) || (scancode == (0x36 | 0x80))) {   /* let up shift key */
//        is_shifted = 0;
//    }
//    /* otherwise, if this is not the release of a key ... */
//    else if ( !(scancode & 0x80) ) {
//        /* If we are in shifted mode, see if we know about the shifted key.  If so, print it in text mode.
//         * If we are not in shifted mode, see if we know about the unshifted key, and if so, print it in text mode.
//         * Whether in shifted mode or not, if we don't know about the key, print the hex value of the scancode */
//        if (us101_scancodes[scancode]) {
//            if (is_shifted)
//                if (shifted_us101_scancodes[scancode])
//                    textmode_putc( shifted_us101_scancodes[scancode] );
//                else
//                    textmode_put_hexbyte( scancode );
//            else
//                textmode_putc( us101_scancodes[scancode] );
//        }
//        else {
//            textmode_put_hexbyte( scancode );
//        }
//    }
//}

void keyboard_irq_install() {
    keyboard_buffer_queue.limit   = KEYBOARD_QUEUE_SIZE - 1;
    keyboard_buffer_queue.slots   = kb_slots;
    keyboard_buffer_queue.top     = 0;
    keyboard_buffer_queue.bottom  = 0;

    irq_install_handler( 1, keyboard_handler );
}
