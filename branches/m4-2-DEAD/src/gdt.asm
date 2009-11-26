[BITS 32]

SECTION .text

global create_gdt

; GRUB creates a GDT, but we don't know where.  So, to avoid tromping on it -- which would nuke the processor (reboots will happen!) --
; we'll create our own here.  Table has null segment (offset 0x00), code segment (0x08) and data segment (0x10)
; Frankly, I find this easier -- and more readable -- in assembly than C, so that's how I'll do it here
create_gdt:
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
    ret


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
