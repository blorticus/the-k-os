[BITS 32]

SECTION .text

global null_interrupt_handler

; this is an interrupt handler that does nothing
null_interrupt_handler:
    iret
