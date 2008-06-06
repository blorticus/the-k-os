; kos-silly-loader
; version 0.0.1
; This loader is quite simple.  It jumps to code that assumes
; the kernel to be loaded starts at the first block past the boot
; block (at 0x200 -- 512 bytes past the start of this block).  This
; boot loader sucks.  Get over it.

[bits 16]
[org 0]
zero:
    cli                     ; disable interrupts
    jmp 07C0h:start         ; rigorize segment


msg:    db  "The K-OS", 13, 10, 0  ; message plus CR/LF and NULL

start:
    ; data segment and extra segment offset addrs here (0x07c0)
    mov ax, cs
    mov ds, ax
    mov es, ax
    
    ; use int 10h sub 0Eh prints one character.  Repeat over length of
    ; [msg], stopping when end of string (NULL) is reached
    mov si, msg

print:
    lodsb               ; ds:si copied to al (used by int 10h 0Eh), then increment ds:si

    cmp al, 0           ; are we at null?
    je hang             ; ... if so, we're done

    mov ah, 0Eh         ; ... otherwise, set 0xeh mode for int 10h
    int 10h             ; which prints character at ds:si

    jmp print           ; ... and continue to next character

hang:
    jmp hang            ; just hang around forever


end:
    TIMES 510-($-$$) DB 0 ; fill to 510 bytes, 0x1fe
    dw 0xAA55             ; boot sector magic number

;; boot sector must end at 512 bytes
