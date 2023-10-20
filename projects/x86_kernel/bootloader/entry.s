bits 16

%define CRLF 0x0D, 0x0A

section _ENTRY class=CODE

extern _cstart
global entry

; --------------------
; entry point
; --------------------
entry:
    mov si, BootloaderEntryMessage
    call PutString

    cli
    mov ax, ds
    mov ss, ax
    mov sp, 0
    mov bp, sp
    sti

    xor dh, dh
    push dx
    call _cstart

    ; _cstart should not return
    cli
    hlt

; --------------------
; prints a string
; in:
;       si -> address of string
; --------------------
PutString:
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

; --------------------
; strings
; --------------------
BootloaderEntryMessage:
    db 'Bootloader Entered.', CRLF, 0