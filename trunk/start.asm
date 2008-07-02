[BITS 32]

global start

start:

segments_set:
    mov esp, sys_stack

    extern kmain
    call kmain

    jmp $



SECTION .bss
    resb 8192
sys_stack:
