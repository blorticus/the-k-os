#include <input/keyboard.h>
#include <platform/ia-32/interrupts.h>
#include <platform/ia-32/asm.h>
#include <sys/types.h>
#include <video/kterm.h>

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


u8 is_shifted = 0;


/* count of characters to enqueue from keyboard in circular buffer,  CANNOT BE LARGER THAN 2**16 - 1 */
#define KEYBOARD_QUEUE_SIZE     128


/* The actual keyboard character queue */
ScCQueue keyboard_buffer_queue;

u16 kb_slots[KEYBOARD_QUEUE_SIZE];  /* without kmalloc() yet, pre-allocate keyboard buffer.  Later this should be done when IRQ is installed */


/**
 * keybaord_handler takes the standard ISR parameter (the register values).  Perform byte read on I/O port 0x60.  This is the scancode produced
 * by the keypress.  Write it to the top of the queue.
 **/
static void keyboard_handler( struct regs *r ) {
    int scancode;

    scancode = ioport_readb( 0x60 );

    keyboard_buffer_queue.slots[keyboard_buffer_queue.top++ % keyboard_buffer_queue.size] = scancode;

    if (keyboard_buffer_queue.top == 0) {
        /* counter wrapped, so re-adjust both top and bottom counters such that they point to the actual top & bottom slot counts */
        keyboard_buffer_queue.top = keyboard_buffer_queue.adjust;
        keyboard_buffer_queue.bottom = keyboard_buffer_queue.bottom % keyboard_buffer_queue.size;
    }
}



/**
 * Read the next scancode in the scancode buffer.  If it is a non-meta ("character") key being depressed,
 * set lower half of return value to the ASCII value for that key.  If it is a meta key (shift lock,
 * shift keys, control keys, or meta/alt keys) set upper half to the appropriate MetaKey type value.  If
 * a meta key is held down while a character key is pressed, both upper and lower will be set.  If the
 * meta key is a shift key, or the caps lock is down, character keys will be shifted ASCII value.  If the
 * scancode buffer is empty, return 0x0000
 **/

/* bit field indicating what meta key is currently depressed */
u8 G_meta_keys = 0x00;

u16 read_next_key_stroke( u16* scancodes ) {
    u16 scancode;

    char c = '\0';

    if (keyboard_buffer_queue.bottom == keyboard_buffer_queue.top)    /* queue is empty */
        return 0x0000;

    /* ASSERT: this never wraps because of its adjustment in the isr */
    scancode = keyboard_buffer_queue.slots[keyboard_buffer_queue.bottom++ % keyboard_buffer_queue.size];  

    *scancodes = scancode;

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


void keyboard_irq_install() {
    keyboard_buffer_queue.size    = KEYBOARD_QUEUE_SIZE;
    keyboard_buffer_queue.slots   = kb_slots;
    keyboard_buffer_queue.top     = 0;
    keyboard_buffer_queue.bottom  = 0;
    keyboard_buffer_queue.adjust  = (u16)((keyboard_buffer_queue.top - 1) % (u32)keyboard_buffer_queue.size);  /* wrap counter backwards, then calculate modulo */

    irq_install_handler( 1, keyboard_handler );
}
