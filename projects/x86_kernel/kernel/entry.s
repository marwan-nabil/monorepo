org 0x0000
bits 16

%define CRLF 0x0D, 0x0A

; ================================================================= ;
;                         code
; ================================================================= ;
; --------------------
; entry point
; --------------------
Entry:
    mov si, HelloWorldMessage
    call PutString

.Halt:
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

; ================================================================= ;
;                         data
; ================================================================= ;
HelloWorldMessage:
    db 'Hello World, From Kernel', CRLF, 0