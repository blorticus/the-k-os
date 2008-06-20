[BITS 32]
[org 0x2000]

start:
    mov ax, 10h
    mov ds, ax
    mov ss, ax
    mov esp, 090000h
    mov byte [ds:0B8320h], 'K'
    mov byte [ds:0B8321h], 1Bh
    mov byte [ds:0B8322h], 'O'
    mov byte [ds:0B8323h], 1Bh
    mov byte [ds:0B8324h], 'S'
    mov byte [ds:0B8325h], 1Bh

    jmp $
