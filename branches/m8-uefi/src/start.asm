[BITS 64]

; BRIEF         : This is the first entry point for the kernel, loaded by the bootloader.
; ASSUMES       : interrupts are disabled.  Mode is 64-bit long-mode
; DESCRIPTION   : sets up small system stack (current 8KB), then jumps to kernel_start,
;                 the C kernel entry point

; On entry, interrupts are disabled

SECTION .text

global start

start:
segments_set:
    ; setup a small system stack (8kB)
    mov rsp, sys_stack
    jmp kentry

kentry:
    extern kernel_start
    call kernel_start

    jmp $


SECTION .bss
    resb 8192
sys_stack:
