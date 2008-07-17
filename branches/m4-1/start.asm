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


; GRUB creates a GDT, but we don't know where.  So, to avoid tromping on it -- which would nuke the processor (reboots will happen!) --
; we'll create our own here.  Table has null segment (offset 0x00), code segment (0x08) and data segment (0x10)
create_gdt:
    xor ax, ax          ; sets ax to 0
    mov ds, ax          ; base for GDT is 0
    lgdt [gdt_ptr]      ; load table pointer

    mov ax, 0x10        ; 0x10 == the third GDT table entry == the data segment selector
    mov ds, ax          ; so, point all data segment registers there
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax          ; including the stack segment register


; we also must make sure we're pointing to code segment, so we do a far jump (that is, we jump through a specific segment selector)
; "far" is funny when we're jumping ahead so close
code_jump:
    jmp 0x08:jmp_point

jmp_point:


kentry:
    extern kmain
    call kmain

    jmp $


SECTION .data

;; Global Descriptor Table (http://www.osdever.net/bkerndev/Docs/gdt.htm) pointer
;; and table
gdt_ptr:
    dw gdt_table_end - gdt_table_start - 1
    dd gdt_table_start

gdt_table_start:
    ;; Each entry is 64-bits long, including:
    ;;   16 bits  : limit
    ;;   24 bits  : base (low/middle)
    ;;    8 bits  : access
    ;;               - 1 bit     : present (always 1)
    ;;                 2 bits    : privilege level (0 or 3)
    ;;                 1 bit     : always 1
    ;;                 1 bit     : segment is executable (i.e., is code segment)
    ;;                 1 bit     : direction (for data) or conforming (for code)
    ;;                 1 bit     : readable (code), writable (data)
    ;;                 1 bit     : access (set to 0, but set by CPU)
    ;;    8 bits  : flags
    ;;               - 1 bit     : granularity (0 == 1 byte; 1 == 4KiB [page])
    ;;               - 1 bit     : size (0 == 16bit; 1 == 32bit)
    ;;               - [remainder are 1]
    ;;    8 bits  : base (high)


    ;; Required, but not directly used
    gdt_null:
    dw 0, 0, 0, 0


    ;; Entry for "code" section of flat model.  Start point is 0x08
    gdt_code_entry:
    dw 0xFFFF           ; limit (in 4KiB pages)
    dw 0x0000           ; base low
    db 0                ; base middle
    db 10011010b        ; access
    db 11001111b        ; granularity
    db 0                ; base high


    ;; Entry for "data" section of flat model.  Start point is 0x10
    dw 0xFFFF           ; limit
    dw 0x0000           ; base low
    db 0                ; base middle
    db 10010010b        ; access
    db 11001111b        ; granularity
    db 0                ; base high

gdt_table_end:    ; used so that NASM can compute the length, in case I change it later



SECTION .bss
    resb 8192
sys_stack:
