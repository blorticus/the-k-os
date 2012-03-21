[BITS 32]

SECTION .text

;
; DESCRIPTION           : install_gdt loads the global descriptor table data structure pointer into the GDT register
; RATIONALE             : install_gdt is written in assembly because, after executing 'lgdt' to  load the
;                         structure ptr into the GDT register, it is necessary to execute a far jump in order to
;                         begin utilizing the code segment gate

global install_gdt
extern gdt_table_info

install_gdt:
    lgdt [gdt_table_info]

    mov ax, 0x10        ; 0x10 == the third GDT table entry == the data segment selector
    mov ds, ax          ; so, point all data segment registers there
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax          ; including the stack segment register

; we also must make sure we're pointing to code segment, so we do a far jump (that is, we jump through a specific segment selector)
; "far" is funny when we're jumping ahead so near
;code_jump:
    jmp 0x08:jmp_point

jmp_point:
    ret
