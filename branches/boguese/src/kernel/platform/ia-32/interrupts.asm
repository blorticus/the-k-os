[BITS 32]

SECTION .text

;------------
; 
; An ISR is an Interrupt Service Routine called when an interrupt is raised.  As explained in
; src/kernel/platform/ia-32/idt.c, the interrupt source can be from hardware, as a processor
; exception or as a software interrupt.  Regardless, an interrupt checks the Interrupt Descriptor
; Table (IDT) and calls the code associated.  Generally, before the actual routine can
; execute, the state of the interrupted routine must be pushed to the stack.  After the execution
; of the ISR, the state is usually restored and 'iret' is called.  I have not figured out
; how to do this cleanly using gcc c code, so it is done using assembly.
;-------------


;
; SYNOPSIS          : STANDARD_ISR( method )
; DESCRIPTION       : Save state of interrupted routine to the stack, switch data registers
;                     to GDT_DPL_0_DATA_SEGMENT_OFFSET (defined in platform/ia-32/gdt.h),
;                     call 'method', restore data registers, restore remaining registers, then
;                     'iret'

%macro STANDARD_ISR 1
    cli
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, %1
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret
%endmacro


;
; SYNOPSIS              : TRIVIAL_ERROR_ISR( isr_num )
; DESCRIPTION           : Executes method 'trivial_error_isr' in platform/ia-32/isr.c
;

extern soft_int_handler
%macro TRIVIAL_ERROR_ISR 1
    cli
    push byte 0
    push byte %1
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, soft_int_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
%endmacro



;----------
; ISR DEFINITIONS
;
; For each ISR, create a label, insert code or use a macro above, and declar the label 'global' to make
; it visible elsewhere
;----------

global interrupt_0

; Exception: Divide by Zero
interrupt_0:    TRIVIAL_ERROR_ISR( 0 )
