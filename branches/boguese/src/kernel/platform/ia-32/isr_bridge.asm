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

extern exception_handler
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
    mov eax, exception_handler
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

global interrupt_00
global interrupt_01
global interrupt_02
global interrupt_03
global interrupt_04
global interrupt_05
global interrupt_06
global interrupt_07
global interrupt_08
global interrupt_09
global interrupt_10
global interrupt_11
global interrupt_12
global interrupt_13
global interrupt_14
global interrupt_15
global interrupt_16
global interrupt_17
global interrupt_18
global interrupt_19
global interrupt_20
global interrupt_21
global interrupt_22
global interrupt_23
global interrupt_24
global interrupt_25
global interrupt_26
global interrupt_27
global interrupt_28
global interrupt_29
global interrupt_30
global interrupt_31


interrupt_00:   TRIVIAL_ERROR_ISR( 00 )  ; EXCEPTION: #DE Divide Error
interrupt_01:   TRIVIAL_ERROR_ISR( 01 )  ; EXCEPTION: #DB Debug
interrupt_02:   TRIVIAL_ERROR_ISR( 02 )  ; EXCEPTION: Non-Maskable Interrupt
interrupt_03:   TRIVIAL_ERROR_ISR( 03 )  ; EXCEPTION: #BP Breakpoint 
interrupt_04:   TRIVIAL_ERROR_ISR( 04 )  ; EXCEPTION: #OF Overflow
interrupt_05:   TRIVIAL_ERROR_ISR( 05 )  ; EXCEPTION: #BR Bound Range Exceeded
interrupt_06:   TRIVIAL_ERROR_ISR( 06 )  ; EXCEPTION: #UD Invalid/Undefined Opcode
interrupt_07:   TRIVIAL_ERROR_ISR( 07 )  ; EXCEPTION: #NM No Math Coprocessor Available
interrupt_08:   TRIVIAL_ERROR_ISR( 08 )  ; EXCEPTION: #DB Double Fault
interrupt_09:   TRIVIAL_ERROR_ISR( 09 )  ; EXCEPTION: Coprocessor Segment Overrun
interrupt_10:   TRIVIAL_ERROR_ISR( 10 )  ; EXCEPTION: #TS Invalid TSS
interrupt_11:   TRIVIAL_ERROR_ISR( 11 )  ; EXCEPTION: #NP Segment Not Present
interrupt_12:   TRIVIAL_ERROR_ISR( 12 )  ; EXCEPTION: #SS Stack Segment Fault
interrupt_13:   TRIVIAL_ERROR_ISR( 13 )  ; EXCEPTION: #GP General Protection Fault
interrupt_14:   TRIVIAL_ERROR_ISR( 14 )  ; EXCEPTION: #PF Page Fault
interrupt_15:   TRIVIAL_ERROR_ISR( 15 )  ; EXCEPTION: (Reserved)
interrupt_16:   TRIVIAL_ERROR_ISR( 16 )  ; EXCEPTION: #MF Floating Point Unit (FPU) Error
interrupt_17:   TRIVIAL_ERROR_ISR( 17 )  ; EXCEPTION: #AC Alignment Check Exception
interrupt_18:   TRIVIAL_ERROR_ISR( 18 )  ; EXCEPTION: #MC Machine Check Exception
interrupt_19:   TRIVIAL_ERROR_ISR( 19 )  ; EXCEPTION: #XM SIMD Floating Point Exception
interrupt_20:   TRIVIAL_ERROR_ISR( 20 )  ; EXCEPTION: (Reserved) 
interrupt_21:   TRIVIAL_ERROR_ISR( 21 )  ; EXCEPTION: (Reserved)
interrupt_22:   TRIVIAL_ERROR_ISR( 22 )  ; EXCEPTION: (Reserved)`
interrupt_23:   TRIVIAL_ERROR_ISR( 23 )  ; EXCEPTION: (Reserved)
interrupt_24:   TRIVIAL_ERROR_ISR( 24 )  ; EXCEPTION: (Reserved)
interrupt_25:   TRIVIAL_ERROR_ISR( 25 )  ; EXCEPTION: (Reserved)
interrupt_26:   TRIVIAL_ERROR_ISR( 26 )  ; EXCEPTION: (Reserved)
interrupt_27:   TRIVIAL_ERROR_ISR( 27 )  ; EXCEPTION: (Reserved)
interrupt_28:   TRIVIAL_ERROR_ISR( 28 )  ; EXCEPTION: (Reserved)
interrupt_29:   TRIVIAL_ERROR_ISR( 29 )  ; EXCEPTION: (Reserved)
interrupt_30:   TRIVIAL_ERROR_ISR( 30 )  ; EXCEPTION: (Reserved)
interrupt_31:   TRIVIAL_ERROR_ISR( 31 )  ; EXCEPTION: (Reserved)

extern isr_test
global interrupt_128
interrupt_128:  STANDARD_ISR( isr_test )
