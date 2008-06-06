start:
    mov ax, cs
    mov ds, ax
    mov es, ax

    mov si, msg

    call PrintString

hang:
    jmp hang            ; just hang around forever

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

msg:    db  "The K-OS", 13, 10, 0  ; message plus CR/LF and NULL

end:
    TIMES 512-($-$$) DB 0 ; fill to end of containing sector
