#ifndef __INPUT_KEYBOARD_H__
#define __INPUT_KEYBOARD_H__

#include <sys/types.h>


/**
 * BRIEF:           A basic US-101 keyboard driver
 * BACKGROUND:      Methods in this package install a keyboard IRQ ISR.  When a key is pressed, and the IRQ is raised, another method
 *                  reads the produced scancodes for the appropriate I/O port, and maps the scancode(s) to actual key(s)
 * SEE:             http://wiki.osdev.org/PS2_Keyboard
 * RE-ENTRANT?:     NO
 ***/


/* so-called ancillary keys that might be depressed at the same time as a character key */
#define CAPS_LOCK       0x01
#define LEFT_SHIFT      0x02
#define LEFT_CTRL       0x04
#define LEFT_META       0x08
#define RIGHT_META      0x10
#define RIGHT_CTRL      0x20
#define RIGHT_SHIFT     0x40


/**
 *
 * DESCRIPTION:     Keyboard buffer, stores scancodes in a circular buffer.  This is also the buffer from which scancodes are read, so careful coordination
 *                  between those two functions is necessary.
 * ELEMENTS:        slots = the actual scancode
 *                  size = number of scancodes that can be stored in the buffer
 *                  adjust = XXX: need to read code to figure this out
 *                  top = index in 'slots' where last character was written (next write will be at top + 1 or 0 if top == size - 1)
 *                  bottom = index in 'slots' where last character was read (next read will be at bottom + 1 if there are any more characters to be read)
 */ 
typedef struct {
    u16*            slots;
    u16             size;       /* number of slots */
    u16             adjust;     /* MAX(top) % size, or the new offset for top when it wraps to 0 */
    s16             top;        /* index of last character written to queue */
    s16             bottom;     /* index of last character read from queue */
} ScCQueue;


/**
 *
 * DESCRIPTION:     read the next scancode in the keyboard queue, convert it into meta and character, and return that.  'scancodes' is a pointer to the
 *                  actual list of scancodes corresponding to the key.  Some keystrokes produce more than one byte, but some produce two.  The consumer
 *                  must simply know which is which based on the first byte returned.
 * RETURN:          the upper byte is a set of eight flags corresponding to the ancillary keys defined above.  Thus, if the left control key and the
 *                  right shift key were depressed, the upper byte will be LEFT_CTRL | RIGHT_SHIFT.  The lower byte is the ASCII character.  If either
 *                  shift key was depressed along with a character, it's keyboard shifted value will be expressed.  'scancodes' may contain only an
 *                  ancillary key, in which case 'scancodes' will have a value (and the top half of the returned byte will have a corresponding value),
 *                  but the bottom byte of the return value will be 0x00
 * SIDE-EFFECTS:    advance the buffer pointer of read scancodes
 * NOTES:           none
 * RE-ENTRANT?:     NO
 *
 */ 
u16 read_next_key_stroke( u16* scancodes );


/**
 *
 * DESCRIPTION:     Install the IRQ ISR for keyboard input.  This method populates a global ScCQueue, the same one from which read_next_key_stroke() reads
 * RETURN:          void
 * SIDE-EFFECTS:    replaces ISR for keyboard IRQ
 * NOTES:           none
 * RE-ENTRANT?:     YES
 *
 */ 
void keyboard_irq_install();


#endif
