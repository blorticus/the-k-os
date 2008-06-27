[BITS 32]

global start, silly_kos

start:

segments_set:
;    mov ax, 08h
;    mov cs, ax
    mov ax, 10h
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, sys_stack

    extern kmain
    call kmain

    jmp $


SECTION .bss
    resb 8192
sys_stack:
