; kos-silly-loader
; version 0.0.4
; This loader is quite simple.  It:
;   1. sets up a local stack
;   2. loads 2nd sector off of current disk to fixed memory location
;   3. configures a base GDT (3 descriptors: null, code, data)
;   4. sets A20 address line
;   5. sets cr0 for protected mode
;   6. far jump to fixed memory location
; After jump to new kernel, no IDT set, only base GDT set, interrupts are disabled
; This boot loader sucks.  Get over it.

[bits 16]
[org 0x7C00]

;;
;; Reset segment selectors after jumping here to rigorize segment base address
;; We do this because the BIOS may jump to any of a number of base addresses,
;; as long as we're at 07C0h.  So we perform a jump to a location where 07C0h
;; is the base address.  (NOTE: we don't do the jump, but might fix that in a bit)
;;
zero:

;;;     jmp 07C0h:start         ; rigorize segment
;;;     cli         ; disable interrupts
;;;
     mov ax, cs
     mov ds, ax
     mov es, ax

     sti
 

start:
    mov si, boot_msg
    call PrintString


;;
;; Setup a local stack.  BIOS has done so, but using it is purportedly unpredictable.
;; The stack is at 0x9000:0 and is 8192 (0x2000) bytes large (grows down)
;;
set_stack:
    mov ax, 0x9000
    mov es, ax
    mov sp, 0x2000


;;
;; Using BIOS routines, read adjascent sector into memory (to fixed 0x2000)
;;
reset_drive:
    mov dl, 0           ; floppy drive 0
    mov ah, 0           ; int 13h 00h resets drive specified at dl
    int 13h
    jc reset_failure    ; if we failed...

load_sector:
    mov ax, 0x1000
    mov es, ax          ; put data at 1MB
    xor bx, bx          ; offset 0
    mov ah, 2           ; int subcommand: int 13h 02h reads sectors from a disk
    mov al, 1           ; read one sector
    xor ch, ch          ; cylinder 0 (starts at 0)
    mov cl, 2           ; sector 2 (starts at 1)
    xor dh, dh          ; head 0 (starts at 0)
    xor dl, dl          ; drive 0 (first floppy drive)

    int 13h

    jc  failed_load     ; carry flag == 0 if int 13h 02h successful
    jmp load_successful


failed_load:
    mov si, floppy_fail_msg
    jmp reset_drive


load_successful:
    mov si, floppy_success_msg
    call PrintString 
    jmp create_gdt


reset_failure:
    mov si, reset_failure_msg
    call PrintString
    jmp $


;;
;; Create Global Descriptor Table (GDT) -- see below for details
;;
create_gdt:
    xor ax, ax
    mov ds, ax          ; base for GDT is 0

    lgdt [gdt_ptr]      ; load table pointer

    mov si, gdt_set_msg
    call PrintString


;;
;; Enable the A20 address line for full memory access.  Read this for
;; gorey details: http://www.osdev.org/wiki/A20_Line.  The code
;; is lifted from there.
;;
enable_A20:
        cli

        call    a20wait
        mov     al,0xAD
        out     0x64,al

        call    a20wait
        mov     al,0xD0
        out     0x64,al

        call    a20wait2
        in      al,0x60
        push    eax

        call    a20wait
        mov     al,0xD1
        out     0x64,al

        call    a20wait
        pop     eax
        or      al,2
        out     0x60,al

        call    a20wait
        mov     al,0xAE
        out     0x64,al

        call    a20wait
        sti
        jmp     a20done

a20wait:
        in      al,0x64
        test    al,2
        jnz     a20wait
        ret


a20wait2:
        in      al,0x64
        test    al,1
        jz      a20wait2
        ret


a20done:
    mov si, a20_activated_msg
    call PrintString


;;
;; Switch to 32-bit protected mode
;;
to_32bit_pm:
    ; disable standard interrupts
    cli

    ; disable IRQs -- it's unclear whether this is strictly necessary
    mov al, 0xFF
    out 0x21, al

    ; disable non-maskable interrupts -- it's also unclear if this is strictly necessary
    ; and actually ... with these two disabled we could miss important hardware interrupts...
    in al, 0x70
    or al, 10000000b
    out 0x70, al

    ; enter protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

;;
;; Jump to relocated kernel
;;
relocate:
    mov ax, 10h
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
;    jmp 08h:2000h   ; REMEMBER: 0x08 is the first byte of the code selector in the GDT
    jmp dword 8:10000h



; ------------------------------------ ;


;; -------------
;; Service Routines
;; -------------


PrintString:
    ;; use BIOS int 10h 0Eh to print message at ds:si
    ;;
    lodsb           ; ds:si copied to al, then increment ds:si

    cmp al, 0       ; are we at null?
    je .Return      ; ... if so, we're done

    mov ah, 0Eh     ; ... otherwise, set 0xeh mode for int 10h
    int 10h         ; which prints character at ds:si

    jmp PrintString ; ... and continue to next character

.Return:
    ret


;; -------------
;; Various Messages for output
;;  , 13, 10, 0 is "line feed", "carriage return", "null"
;; -------------
boot_msg:           db  "Silly Bootloader v0.0.3", 13, 10, 0

floppy_fail_msg:    db  "Floppy Read Failure!!", 13, 10, 0

floppy_success_msg: db  "Read First Sector From Floppy", 13, 10, 0

reset_failure_msg:  db  "reset failed", 13, 10, 0

gdt_set_msg:        db  "GDT Configured", 13, 10, 0

a20_activated_msg:  db  "A20 Address Line Activated", 13, 10, 0

pm32_activated_msg: db  "32-Bit Protected Mode Activated", 13, 10, 0


;; -------------
;; Data structures
;; -------------

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


end:
    TIMES 510-($-$$) DB 0 ; fill to 510 bytes, 0x1fe
    dw 0xAA55             ; boot sector magic number

;; boot sector must end at 512 bytes
