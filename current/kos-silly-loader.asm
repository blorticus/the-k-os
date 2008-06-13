; kos-silly-loader
; version 0.0.3
; This loader is quite simple.  It creates an IDT, a GDT, set the A20 gate,
; probes for upper memory, relocates basic BIOS info, loads the next sector
; off of the disk, then jumps to the new memory location.
; This boot loader sucks.  Get over it.

[bits 16]
[org 0]
zero:
    jmp 07C0h:start         ; rigorize segment


;;
;; Reset segment selectors after jumping here to rigorize segment base address
;; We do this because the BIOS may jump to any of a number of base addresses,
;; as long as we're at 07C0h.  So we perform a jump to a location where 07C0h
;; is the base address.
;;
start:
    cli         ; disable interrupts

    mov ax, cs
    mov ds, ax
    mov es, ax
    
    mov si, boot_msg

    call PrintString


;;
;; Setup a local stack.  BIOS has done so, but using it is purportedly unpredictable
;; The stack is at 0x9000:0 and is 8192 (0x2000) bytes large
;;
set_stack:
    mov ax, 0x9000
    mov es, ax
    mov sp, 0x2000

    sti             ; enable interrupts


;;
;; Using BIOS routines, read adjascent sector into memory (still at 0x1000:0), then jump
;; to that location
;;
reset_drive:
    mov dl, 0           ; floppy drive 0
    mov ah, 0           ; int 13h 00h resets drive specified at dl
    int 13h
    jc reset_failure

load_sector:
    mov ax, 1000h
    mov es, ax          ; put data in segment 0x1000
    xor bx, bx          ; offset 0
    mov ah, 2           ; int 13h 02h reads sectors from a disk
    mov al, 1           ; read one sector
    xor ch, ch          ; cylinder 0 (starts at 0)
    mov cl, 2           ; sector 2 (starts at 1)
    xor dh, dh          ; head 0 (starts at 0)
    xor dl, dl          ; drive 0 (first floppy drive)

    int 13h

    jc  failed_one_load ; carry flag == 0 if int 13h 02h successful

    jmp load_successful

failed_one_load:        ; ... otherwise, we failed to load the sector
    mov si, floppy_fail_msg
    jmp reset_drive

cannot_load:
    mov si, floppy_fail_msg
    call PrintString
    jmp $               ; hang forever :(

load_successful:
    mov si, floppy_success_msg
    call PrintString 
    jmp 1000h:0000

reset_failure:
    mov si, reset_failure_msg
    call PrintString
    jmp $


;;
;; Create Interrupt Descriptor Table (IDT)
;;
create_idt:



;;
;; Create Global Descriptor Table (GDT)
;;
create_gdt:
    lgdt [gdt_descriptor]



;;
;; Enable the A20 gate for full memory access.  Read this for
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
        pop     ax
        or      al,2
        out     0x60,al

        call    a20wait
        mov     al,0xAE
        out     0x64,al

        call    a20wait
        sti
        ret

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



;;
;; Switch to 32-bit protected mode
;;



;;
;; 


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
boot_msg:           db  "Silly Bootloader...", 13, 10, 0

floppy_fail_msg:    db  "Floppy Read Failure!!", 13, 10, 0

floppy_success_msg: db  "Read First Sector From Floppy", 13, 10, 0

load_success_msg:   db  "(relocate successful)", 13, 10, 0

reset_failure_msg:  db  "reset failed", 13, 10, 0


;; -------------
;; Data structures
;; -------------
    ;; Global Descriptor Table length, then address of GDT, will be fed to lgdt instruction
gdt_descriptor:
    dw end_gtd_entries - gdt_entries - 1    ; length of GDT
    dd gdt_entries                          ; start address for GDT

gdt_entries:
    ;; Global Descriptor Table (http://www.osdever.net/bkerndev/Docs/gdt.htm)
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
    ;;
    ;; The first descriptor is the null descriptor, and can have any value
    ;; but is generally set to all zeros


    ; the null descriptor...
    dw 0, 0, 0, 0


    ; the code segment, contains all of addressable memory, makes this "flat"
    ; model
    dw 0xFFFF           ; limit (in 4KiB pages)
    dw 0x0000           ; base low
    dw 0x009A           ; base middle + present, priv 0, executable, writable
    dw 0xCF00           ; page granularity, 32bit size + base high 
    

    ; the data segment, contains all of addressable memory
    dw 0xFFFF           ; limit
    dw 0x0000           ; base low
    dw 0x0092           ; base middle + present, priv 0, non-exec, readable
    dw 0xCF00           ; page granularity, 32bit size + base high

end_gtd_entries:    ; used so that NASM can compute the length, in case I change it later


end:
    TIMES 510-($-$$) DB 0 ; fill to 510 bytes, 0x1fe
    dw 0xAA55             ; boot sector magic number

;; boot sector must end at 512 bytes
