; kos-silly-loader
; version 0.0.2
; This loader is quite simple.  It jumps to code that assumes
; the kernel to be loaded starts at the first block past the boot
; block (the second sector on the disk).  This
; boot loader sucks.  Get over it.

[bits 16]
[org 0]
zero:
    jmp 07C0h:start         ; rigorize segment


start:
    ; data segment and extra segment offset addrs here (0x07c0)
    mov ax, cs
    mov ds, ax
    mov es, ax
    
    mov si, boot_msg

    call PrintString

reset_drive:
    mov dl, 0           ; floppy drive 0
    mov ah, 0           ; int 13h 00h resets drive specified at dl
    int 13h
    jc reset_failure

load_sector:
    mov ax, 1000h
    mov es, ax          ; put data in segment 0x2000
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


load_successful:
;    mov si, floppy_success_msg
;    call PrintString 


relocate:
    jmp 1000h:0000


reset_failure:
    mov si, reset_failure_msg
    call PrintString
    jmp $


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

; floppy_success_msg: db  "Read Sector 1 From Floppy", 13, 10, 0

reset_failure_msg:  db  "reset failed", 13, 10, 0

end:
    TIMES 510-($-$$) DB 0 ; fill to 510 bytes, 0x1fe
    dw 0xAA55             ; boot sector magic number

;; boot sector must end at 512 bytes
