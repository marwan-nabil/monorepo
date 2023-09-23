org 0x7C00
bits 16

%define CRLF 0x0D, 0x0A

start:
    ; initialize segment registers
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; initialize stack
    mov sp, 0x8000

    ; print a message
    mov si, hello_world_message
    call puts

halt:
    jmp halt

; prints a string
puts:
    push si
    push ax
    mov ah, 0x0E
    mov bh, 0

.loop_0:
    ; load signed byte from [ds:si] into al, also increments si
    lodsb
    ; test if al is 0
    or al, al
    jz .done
    ; print the character
    int 0x10
    jmp .loop_0

.done:
    pop ax
    pop si
    ret

hello_world_message:
    db "Hello, world!", CRLF, 0

; pad with 0 until you reach address 0x7DFE
times 510 - ($ - $$) db 0

; boot sector signature
dw 0xAA55