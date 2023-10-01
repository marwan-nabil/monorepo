org 0x7C00
bits 16

%define CRLF 0x0D, 0x0A

; jump instruction, 3 bytes
Entry:
    jmp short Start
    nop

; OEM name, 8 bytes
OEMName:
    db "MARWAN", 0, 0

; BIOS parameter block, 19 bytes
BytesPerSector:
    dw 0xFFFF
SectorsPerCluster:
    db 0xFF
NumberOfReserevedSectors:
    dw 0xFFFF
NumberOfFATs:
    db 0xFF
RootDirectoryEntries:
    dw 0xFFFF
TotalSectors:
    dw 0xFFFF
MediaDescriptor:
    db 0xFF
SectorsPerFAT:
    dw 0xFFFF
SectorsPerTrack:
    dw 0xFFFF
NumberOfHeads:
    dw 0xFFFF
HiddenSectors:
    dw 0xFFFF

; extra data, 1 byte
DriveNumber:
    db 0xFF

; boot sector code & data, 479 bytes
Start:
    ; initialize segment registers
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; initialize stack
    mov sp, 0x8000

    ; print a message
    mov si, HelloWorldMessage
    call PutString

Halt:
    jmp Halt

; prints a string
PutString:
    push si
    push ax
    mov ah, 0x0E
    mov bh, 0
.Loop0:
    ; load signed byte from [ds:si] into al, also increments si
    lodsb
    ; test if al is 0
    or al, al
    jz .Done
    ; print the character
    int 0x10
    jmp .Loop0
.Done:
    pop ax
    pop si
    ret

HelloWorldMessage:
    db "Hello, world!", CRLF, 0

; pad with 0 until you reach address 0x7DFE
times 510 - ($ - $$) db 0

; boot sector signature, 2 bytes
dw 0xAA55