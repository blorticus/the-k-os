[BITS 64]

; BRIEF         : This is the first entry point for the kernel, loaded by the bootloader.
; ASSUMES       : interrupts are disabled.  Mode is 64-bit long-mode
; DESCRIPTION   : sets up small system stack (current 8KB), then jumps to kernel_start,
;                 the C kernel entry point

; On entry, interrupts are disabled

;ORG 0x100000

SECTION .text
global start
extern kmain

start:
    mov rsp, sys_stack
;    mov r8, r9
;    mov rcx, 10240
;
;L1:
;    mov [r8], dword 0x000000ff
;    add r8, 4
;    loop L1

    call kmain

    jmp $


SECTION .bss
resb 8192
sys_stack:
