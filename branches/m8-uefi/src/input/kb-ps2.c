#include <input/kb-ps2.h>
#include <platform/x86_64/interrupts.h>
#include <platform/x86_64/asm.h>
#include <sys/types.h>


/* count of scancodes to enqueue from keyboard in circular buffer,  CANNOT BE LARGER THAN 2**16 - 1 */
#define SCANCODE_QUEUE_SIZE     256
#define SCANCODE_QUEUE_BITMASK  8

/* it may seem odd to use these two variables to (sc_q and SC_HW_REGISTERED_QUEUE) for essentially
   the same thing, but in future implementation, it is possible that sc_q will be created elsewhere
   or via kmalloc(), so this allow some additional flexibility */

static ScHwQueue sc_q;
static u8 scancodes[SCANCODE_QUEUE_SIZE];

static ScHwQueue* SC_HW_REGISTERED_QUEUE;


void hw_sc_queue_init( ScHwQueue* q, u8* scancode_buffer, u8 scancode_buffer_size_bits ) {
    q->scancodes   = scancode_buffer;
    q->bitmask     = 0xffff >> (16 - scancode_buffer_size_bits);;
    q->size        = q->bitmask + 1;
    q->elements    = 0;
    q->bottom      = 0;
    q->top         = 0;
}


void hw_sc_queue_put( ScHwQueue* q, u8 scancode ) {
    q->scancodes[q->top] = scancode;
    q->top = (q->top + 1) & q->bitmask;
    if (q->elements < q->size)
        q->elements++;
    else
        // if queue becomes full, adjust bottom to ensure it is pointing at the real start of the queue
        q->bottom = q->top;
}


u8 hw_sc_queue_get( ScHwQueue* q ) {
    u8 sc;

    if (q->elements == 0)
        return 0x00;

    sc = q->scancodes[q->bottom];

    q->bottom = (q->bottom + 1) & q->bitmask;
    q->elements--;

    return sc;
}


void hw_sc_queue_register( ScHwQueue* q ) {
    SC_HW_REGISTERED_QUEUE = q;
}


ScHwQueue* hw_sc_queue_get_registered() {
    return SC_HW_REGISTERED_QUEUE;
}


/**
 * Method to be provided to an ISR.  Read single scancode from ioport 0x60 and write it
 * to the hardware scancode queue.
 **/
#ifndef TESTING
static void keyboard_handler() {
    hw_sc_queue_put( SC_HW_REGISTERED_QUEUE, ioport_readb( 0x60 ) );
}
#endif


void keyboard_irq_install() {
    hw_sc_queue_init( &sc_q, (u8*)scancodes, SCANCODE_QUEUE_BITMASK );
    hw_sc_queue_register( &sc_q );

#ifndef TESTING
    irq_install_handler( 1, keyboard_handler );
#endif
}
