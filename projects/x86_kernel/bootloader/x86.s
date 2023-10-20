bits 16

section _TEXT class=CODE

; --------------------
; writes a character using a BIOS function
; --------------------
global _x86_PrintCharacter
_x86_PrintCharacter:
    push bp
    mov bp, sp
    push bx

    ; [bp + 0] == previous bp
    ; [bp + 2] == return address
    ; [bp + 4] == first argument: character to print
    ; [bp + 6] == second argument: page
    mov ah, 0x0E
    mov al, [bp + 4]
    mov bh, [bp + 6]

    int 0x10

    pop bx
    mov sp, bp
    pop bp
    ret