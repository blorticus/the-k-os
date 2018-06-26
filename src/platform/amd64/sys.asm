[BITS 64]

SECTION .text

global create_gdt

create_gdt:
    lgdt [gdt_ptr]      ; load table pointer

    mov ax, 0x10        ; 0x10 == the third GDT table entry == the data segment selector
    mov ds, ax          ; so, point all data segment registers there
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax          ; including the stack segment register

    push 0x08
    push qword [jmp_point]

    o64 retf

jmp_point:
    ret


SECTION .data

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
    ;;               - 1 bit     : granularity (irrelevant)
    ;;               - 1 bit     : size (must be 0 in long mode)
    ;;               - 1 bit     : long mode enabled
    ;;               - 1 bit     : AVL
    ;;    8 bits  : base (high)


    ;; Required, but not directly used
    gdt_null:
    dw 0, 0, 0, 0


    ;; Entry for "code" section of flat model.  Start point is 0x08.  Given limit and base, it is all of addressable memory.
    gdt_code_entry:
    dw 0xFFFF           ; limit (in 4KiB pages)
    dw 0x0000           ; base low
    db 0                ; base middle
    db 10011000b        ; access
    db 10101111b        ; granularity
    db 0                ; base high


    ;; Entry for "data" section of flat model.  Start point is 0x10 (8 bytes after 0x08...).  Also all of addressable memory.
    dw 0xFFFF           ; limit
    dw 0x0000           ; base low
    db 0                ; base middle
    db 10010010b        ; access
    db 11001111b        ; granularity
    db 0                ; base high

gdt_table_end:    ; used so that NASM can compute the length, in case I change it later
