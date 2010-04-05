[BITS 32]

; BRIEF         : This is the first entry point for the kernel, loaded by the bootloader.
; ASSUMES       : interrupts are disabled.  Mode is 32-bit protected with limited GDT.
; DESCRIPTION   : sets up small system stack (current 8KB), provides multiboot header, sets up GDT and IDT, then jumps to kmain(),
;                 the C kernel entry point

; On entry, interrupts are disabled

SECTION .text

global start

start:
segments_set:
    ; setup a small system stack (8kB)
    mov esp, sys_stack
    jmp kentry

; Multiboot header
ALIGN 4
multiboot_header:
    MULTIBOOT_PAGE_ALIGN    equ 1<<0
    MULTIBOOT_MEMORY_INFO   equ 1<<1
    MULTIBOOT_HEADER_MAGIC  equ 0x1badb002
    MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
    MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM



kentry:
    extern kmain
    call kmain

    jmp $


global idt_load
extern idtp
idt_load:
    lidt [idtp]
    ret

SECTION .bss
    resb 8192
sys_stack:
