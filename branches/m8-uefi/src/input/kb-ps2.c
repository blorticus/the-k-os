#include <input/kb-ps2.h>
#include <platform/x86_64/interrupts.h>
#include <platform/x86_64/asm.h>
#include <sys/types.h>

enum KEYBOARD_ENCODER {
    KB_READ_INPUT_BUFFER  = 0x60,
    KB_KE_SEND_COMMAND    = 0x64
};

enum KEYBOARD_CONTROLLER {
    KB_STATUS_REGISTER    = 0x61,
    KB_KC_SEND_COMMAND    = 0x64
};

enum KB_STATUS_REGISTERS {
    KB_SR_OUTPUT_BUFFER_READY             = 0x01,
    KB_SR_INPUT_BUFFER_READY              = 0x02,
    KB_SR_SYSTEM_FLAG                     = 0x04,
    KB_SR_INPUT_REGISTER_LAST_VALUE_TYPE  = 0x08,
    KB_SR_KEYBOARD_LOCK_STATUS            = 0x10,
    KB_SR_AUX_OUTPUT_BUFFER_FULL          = 0x20,
    KB_SR_TIMEOUT_FLAG                    = 0x40,
    KB_SR_PARITY_FLAG                     = 0x80
};

static keyboard_scancode_receiver RECEIVER_CALLBACK;
static u8 sc_pass_buffer[6];

void kb_ps2_init( keyboard_scancode_receiver callback ) {
    RECEIVER_CALLBACK = callback;
}


/* the installable ISR */
static void keyboard_handler() {
    u8 i, sr;

    while (1) {
        for (sr = ioport_readb( KB_STATUS_REGISTER ), i = 0; i < 6 && (sr & KB_SR_OUTPUT_BUFFER_READY); i++, sr = ioport_readb( KB_STATUS_REGISTER ))
            SC_PASS_BUFFER[i] = sr;

        RECEIVER_CALLBACK( (u8*)sc_pass_buffer, i );

        if (!(sr & KB_SR_OUTPUT_BUFFER_READY))
            return;
    }
}


void keyboard_irq_install() {
    irq_install_handler( 1, keyboard_handler );
}
