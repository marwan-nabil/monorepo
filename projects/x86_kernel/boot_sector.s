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
    dw 0x0200
SectorsPerCluster:
    db 0x01
NumberOfReserevedSectors:
    dw 0x0001
NumberOfFATs:
    db 0x02
RootDirectoryEntries:
    dw 0x00E0
TotalSectors:
    dw 0x0B40
MediaDescriptor:
    db 0xF0
SectorsPerFAT:
    dw 0x0009
SectorsPerTrack:
    dw 0x0012
NumberOfHeads:
    dw 0x0002
HiddenSectors:
    dd 0x00000000
LargeSectors:
    dd 0x00000000

; extended data, 26 bytes
DriveNumber:
    db 0x00
    db 0 ; reserved
Signature:
    db 0x29
VolumeId:
    dd 0x78563412
VolumeLabel:
    db 'SYSTEM     '
SystemId:
    db 'FAT12   '

; boot sector code & data, 448 bytes
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