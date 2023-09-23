org 0x7C00

start:
    mov ah, 0x0E
    mov al, 'N'
    int 0x10

halt:
    jmp halt

; pad with 0 until you reach address 0x7DFE
times 510 - ($ - $$) db 0

dw 0xAA55