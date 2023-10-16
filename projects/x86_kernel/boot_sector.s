org 0x7C00
bits 16

%define CRLF 0x0D, 0x0A

; --------------------
; jump instruction, 3 bytes
; --------------------
Entry:
    jmp short Start
    nop

; --------------------
; OEM name, 8 bytes
; --------------------
OEMName:
    db "MARWAN", 0, 0

; --------------------
; BIOS parameter block, 19 bytes
; --------------------
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

; --------------------
; extended data, 26 bytes
; --------------------
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

; --------------------
; boot sector code & data, 448 bytes
; --------------------

; --------------------
; entry point
; --------------------
Start:
    ; initialize segment registers
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; initialize stack
    mov sp, 0x8000

    ; read some data from the disk
    ; dl == drive number, set by BIOS
    mov [DriveNumber], dl

    ; LBA == 1, second reserved sector in the data area of the fat12 disk
    mov ax, 1
    mov cl, 1
    mov bx, 0x7E00
    ; bx == start of the next sector after the loaded boot sector

    ; call the disk read function
    ; in:
    ;       ax == logical block address
    ;       cl == number of sectors to read
    ;       dl == driver number
    ;       es:bx == address to write disk data to
    ; call ReadFromDisk

    ; print the hello world message
    mov si, HelloWorldMessage
    call PutString

    jmp Halt

; --------------------
; Floppy Disk Error handler
; --------------------
FloppyError:
    mov si, DiskReadFailedMessage
    call PutString
    jmp WaitForKeyThenReboot

; --------------------
; waits for a keyboard input then reboots
; --------------------
WaitForKeyThenReboot:
    mov ah, 0
    ; call the (wait for key press) BIOS function
    int 16h
    ; jump to the BIOS entry point, same as rebooting
    jmp 0FFFFh:0

; --------------------
; generic error handler
; --------------------
Halt:
    ; disable interrupts
    cli
    hlt

; --------------------
; prints a string
; --------------------
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

; ---------------------------------------
; translates a logical block address to CHS address for disk access
; in:
;       ax -> logical block address
; out:
;       cl[5:0] -> sector number
;       cl[7:6] -> cylinder number[9:8]
;       ch -> cylinder number[7:0]
;       dh -> head number
; ---------------------------------------
TranslateLbaToChs:
    push ax
    push dx

    xor dx, dx
    div word [SectorsPerTrack]
    ; ax == LBA / SectorsPerTrack
    ; dx == LBA % SectorsPerTrack
    inc dx
    ; dx == (LBA % SectorsPerTrack) + 1 == sector number
    mov cx, dx
    ; cx == sector number

    xor dx, dx
    div word [NumberOfHeads]
    ; ax == (LBA / SectorsPerTrack) / NumberOfHeads == cylinder number
    ; dx == (LBA / SectorsPerTrack) % NumberOfHeads == head number
    mov dh, dl
    ; dh == head number

    mov ch, al
    ; ch == cylinder number[7:0]
    shl ah, 6
    ; ah[7:6] == cylinder number[9:8]
    or cl, ah
    ; cl[5:0] == sector number
    ; cl[7:6] == cylinder number[9:8]

    pop ax
    ; ax == saved dx
    mov dl, al
    pop ax
    ret

; ---------------------------------------
; reads sectors from disk
; in:
;       ax == logical block address
;       cl == number of sectors to read
;       dl == driver number
;       es:bx == address to write disk data to
; ---------------------------------------
ReadFromDisk:
    ; save registers that will be touched
    push ax
    push bx
    push cx
    push dx
    push di

    push cx
    call TranslateLbaToChs
    pop ax
    ; al == number of sectors to read

    mov ah, 02h
    ; al == number of sectors to read
    ; ah == BIOS function
    ; cl[5:0] == sector number
    ; cl[7:6] == cylinder number[9:8]
    ; ch == cylinder number[7:0]
    ; dl == driver number
    ; dh == head number
    ; es:bx == address to write disk data to
    mov di, 3
    ; di == 3 == number of retries on failure

.Retry:
    ; save all registers
    pusha
    ; set carry flag
    stc
    ; call bios function
    int 13h
    ; carry flag 0 == success
    ; carry flag 1 == failure
    jnc .Done

    ; disk read failed
    popa
    call DiskReset

    dec di
    test di, di
    jnz .Retry

.Fail:
    ; disk read BIOS function keeps failing
    jmp FloppyError

.Done:
    ; disk read succeeded
    popa

    ; restore touched registers
    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret

; ---------------------------------------
; resets the floppy disk
; in:
;       dl == driver number
; ---------------------------------------
DiskReset:
    pusha

    mov ah, 0
    stc
    int 13h
    jc FloppyError

    popa
    ret

; -------------
; string data
; -------------
HelloWorldMessage:
    db 'Hello, world!', CRLF, 0

DiskReadFailedMessage:
    db 'ERROR: failed to read from disk', CRLF, 0

; ---------------------------------------
; pad with 0 until you reach address 0x7DFE
; ---------------------------------------
times 510 - ($ - $$) db 0

; -----------------
; boot sector signature, 2 bytes
; -----------------
dw 0xAA55