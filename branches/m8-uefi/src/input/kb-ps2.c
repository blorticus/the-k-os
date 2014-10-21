#include <input/kb-ps2.h>
#include <platform/x86_64/interrupts.h>
#include <platform/x86_64/asm.h>
#include <sys/types.h>


/* count of scancodes to enqueue from keyboard in circular buffer,  CANNOT BE LARGER THAN 2**16 - 1 */
#define SCANCODE_QUEUE_SIZE     256

/* it may seem odd to use these two variables to (sc_q and SC_HW_REGISTERED_QUEUE) for essentially
   the same thing, but in future implementation, it is possible that sc_q will be created elsewhere
   or via kmalloc(), so this allow some additional flexibility */

static ScHwQueue sc_q;
static u8 scandcodes[SCANCODE_QUEUE_SIZE];

static ScHwQueue* SC_HW_REGISTERED_QUEUE;


void hw_sc_queue_init( ScHwQueue* q, u8* scancode_buffer, u16 scancode_buffer_size ) {
    q->scancodes = scancode_buffer;
    q->size      = scancode_buffer_size;
    q->bottom    = 0;
    q->top       = 0;
}


void hw_sc_queue_put( ScHwQueue* q, u8 scancode ) {
    q->top = (q->top + 1) % q->size;
    q->scancodes[q->top] = scancode;
}


u8 hw_sc_queue_get( ScHwQueue* q ) {
    if (q->top == q->bottom)
        return 0x00;

    q->bottom = (q->bottom + 1) % q->size;

    return q->scancode[q->bottom];
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
static void keyboard_handler() {
    hw_sc_queue_put( SC_HW_REGISTERED_QUEUE, ioport_readb( 0x60 ) );
}



void keyboard_irq_install() {
    hw_sc_queue_init( sc_q, (u8*)scancodes, SCANCODE_QUEUE_SIZE );
    hw_sc_queue_register( &sc_q );

    irq_install_handler( 1, keyboard_handler );
}
