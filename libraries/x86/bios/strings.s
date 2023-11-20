; --------------------
; writes a character using a BIOS function
; --------------------
section .text
BIOSPrintCharacter:
    push bp
    mov bp, sp

    ; word [bp] == previous bp
    ; word [bp + 2] == return offset
    ; word [bp + 4] == first argument: character to print
    ; word [bp + 6] == second argument: page

    push bx

    mov ah, 0x0E
    mov al, [bp + 4]
    mov bh, [bp + 6]
    int 0x10

    pop bx

    mov sp, bp
    pop bp
    ret

; --------------------
; prints a null terminated string
; in:
;       si -> address of string
; --------------------
section .text
BIOS_PrintString:
    ; save touched register
    push si
    push ax
    push bx

.Loop0:
    ; load signed byte from [ds:si] into al, also increments si
    lodsb
    or al, al
    jz .Done

    ; print the character
    ; BIOS function to print a character
    mov ah, 0x0E
    mov bh, 0
    int 0x10
    jmp .Loop0

.Done:
    pop bx
    pop ax
    pop si
    ret