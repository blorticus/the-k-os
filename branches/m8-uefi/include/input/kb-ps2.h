#ifndef __INPUT_KB_PS2_H__
#define __INPUT_KB_PS2_H__

#include <sys/types.h>


/**
 * BRIEF:           A basic driver of PS/2 style keyboard (generally emulated)
 * BACKGROUND:      Methods in this package install a keyboard IRQ ISR.  When a key is pressed,
 *                  and the IRQ is raised, another method reads the produced scancodes for the
 *                  appropriate I/O port.  The put and get methods are coordinated with respect
 *                  to one another, but not internally, so there really should only be one writer
 *                  and one reader.
 * SEE:             http://wiki.osdev.org/PS2_Keyboard
 * RE-ENTRANT?:     NO
 ***/


/**
 *
 * DESCRIPTION:     Keyboard buffer, stores hardware scancodes in a circular buffer.  This is also
 *                  the buffer from which scancodes are read, so careful coordination between those
 *                  two functions is necessary.
 * ELEMENTS:        scancodes = the hardware scancode stream
 *                  size      = size (the number of scancodes that can be in the queue; must be a power
 *                              of 2 to make the modulo calculation as fast as possible)
 *                  bitmask   = size - 1, used as the actual modulo
 *                  elements  = number of scancodes currently in the queue
 *                  top       = index in 'slots' where last character was written (next write will
 *                              be at top + 1 or 0 if top == size - 1)
 *                  bottom    = index in 'slots' where last character was read (next read will be
 *                              at bottom + 1 if there are any more characters to be read)
 * RE-ENTRANT?:     NO
 */ 
typedef struct {
    u8*             scancodes;
    u8              size;       /* total depth of queue; must be a power of 2 */
    u16             bitmask;    /* contiguous bits used for modulo of size */
    u16             elements;   /* number of scancodes currently in queue */
    u16             top;        /* the element in 'scancodes' the next to be written */
    u16             bottom;     /* the element in 'scancodes' the next to be read */;
} ScHwQueue;


/**
 *
 * DESCRIPTION:     Registers a specific ScHwQueue for use by the keyboard IRQ installer
 *                  keyboard_irq_install().  This must be run before keyboard_irq_install().
 * RETURN:          void
 * SIDE-EFFECTS:    Makes the provided ScHwQueue the one used for hw_sc_queue_put() when
 *                  it is called in the IRQ handler.
 * NOTES:           none
 * RE-ENTRANT?:     NO
 *
 **/
void hw_sc_queue_register( ScHwQueue* q );


/**
 *
 * DESCRIPTION:     Retrieve the registered ScHwQueue.
 * RETURN:          Registered ScHwQueue (or NULL if no ScHwQueue is registered).
 * SIDE-EFFECTS:    none
 * NOTES:           none
 * RE-ENTRANT?:     NO
 *
 **/
ScHwQueue* hw_sc_queue_get_registered();


/**
 *
 * DESCRIPTION:     Initialize an ScHwQueue.  Attach scancode_buffer (which must be
 *                  2**scancode_buffer_size_bits long) to queue.  scancode_buffer should not be
 *                  read or altered outside of the hw_sc_* methods after calling this method.
 * RETURNS:         void
 * SIDE-EFFECTS:    none
 * NOTES:           scancode_buffer_size_bits must be <= 16, but there is no verification.
 * RE-ENTRANT?:     For different ScHwQueue pointers, YES; otherwise, NO.
 *
 **/
void hw_sc_queue_init( ScHwQueue* q, u8* scancode_buffer, u8 scancode_buffer_size_bits );


/**
 *
 * DESCRIPTION:     Insert scancode into scancode hardware queue
 * RETURN:          void
 * SIDE-EFFECTS:    Advance the buffer pointer of written scancodes
 * NOTES:           This method must be synchronized by wrapping it around disabled
 *                  interrupts or the queue may end up incoherent
 * RE-ENTRANT?:     NO, but it must be wrapped in disabled interrupts
 *
 */ 
void hw_sc_queue_put( ScHwQueue* q, u8 scancode );


/**
 *
 * DESCRIPTION:     Read and return the next scancode in the hardware scancode queue.
 * RETURN:          The next hardware scancode or 0x00 when the queue is empty.
 * SIDE-EFFECTS:    Advance the buffer pointer of read scancodes.
 * NOTES:           This method must be synchronzed by wrapping it around disabled
 *                  interrupts or the queue may end up incoherent
 * RE-ENTRANT?:     NO, but it must be wrapped in disabled interrupts
 *
 */ 
u8 hw_sc_queue_get( ScHwQueue* q );


/**
 *
 * DESCRIPTION:     Install the IRQ ISR for keyboard input.  This method populates a global ScCQueue, the
 *                  same one from which read_next_key_stroke() reads
 * RETURN:          void
 * SIDE-EFFECTS:    Replaces ISR for keyboard IRQ
 * NOTES:           None
 * RE-ENTRANT?:     YES
 *
 */ 
void keyboard_irq_install();


#endif
