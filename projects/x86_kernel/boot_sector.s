; BIOS will load the boot sector at this address
org 0x7C00
; Real mode
bits 16

%define CRLF 0x0D, 0x0A

; --------------------
; jump instruction, 3 bytes
; --------------------
Entry:
    jmp short Start
    nop

; ================================================================= ;
;                       boot sector BIOS data
; ================================================================= ;
; --------------------
; OEM name, 8 bytes
; --------------------
OEMName:
    db "MARWAN", 0, 0

; --------------------
; BIOS parameter block, 25 bytes
; --------------------
BytesPerSector:
    dw 512
SectorsPerCluster:
    db 1
NumberOfReserevedSectors:
    dw 1
NumberOfFATs:
    db 2
RootDirectoryEntries:
    dw 0x00E0
TotalSectors:
    dw 2880
MediaDescriptor:
    db 0xF0
SectorsPerFAT:
    dw 9
SectorsPerTrack:
    dw 18
NumberOfHeads:
    dw 2
HiddenSectors:
    dd 0
LargeSectors:
    dd 0

; --------------------
; extended data, 26 bytes
; --------------------
DriveNumber:
    db 0
Reserved:
    db 0
Signature:
    db 0x29
VolumeId:
    dd 12h, 34h, 56h, 78h
VolumeLabel:
    db 'SYSTEM     '
SystemId:
    db 'FAT12   '

; ================================================================= ;
;                       boot sector code
; ================================================================= ;
; --------------------
; real entry point
; --------------------
Start:
    ; initialize segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; initialize stack
    mov ss, ax
    mov sp, 0x7C00

    ; ---------------------
    ; read some data from the disk
    ; ---------------------
    mov ax, 1
    ; ax == logical block address,
    ; second reserved sector in the data
    ; area of the fat12 disk
    mov cl, 1
    ; cl == number of sectors to read
    mov [DriveNumber], dl
    ; dl == drive number, set by BIOS
    mov bx, 0x7E00
    ; es:bx == address to write disk data to,
    ; start of the next sector after the
    ; loaded boot sector
    call ReadFromDisk

    ; print the hello world message
    mov si, HelloWorldMessage
    call PutString

    cli
    hlt

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
; Floppy Disk Error handler
; --------------------
FloppyErrorHandler:
    mov si, DiskReadFailedMessage
    call PutString
    jmp WaitForKeyThenReboot

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
    jmp FloppyErrorHandler

.Done:
    ; disk read succeeded
    popa
    mov si, DiskReadSuccessMessage
    call PutString

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
    jc FloppyErrorHandler

    popa
    ret

; ================================================================= ;
;                         boot sector data
; ================================================================= ;
HelloWorldMessage:
    db 'Hello, world!', CRLF, 0

DiskReadFailedMessage:
    db 'ERROR: failed to read from disk.', CRLF, 0

DiskReadSuccessMessage:
    db 'INFO: read from disk succeeded.', CRLF, 0

; ---------------------------------------
; pad with 0 until you reach address 0x7DFE
; ---------------------------------------
times 510 - ($ - $$) db 0

; -----------------
; boot sector signature, 2 bytes
; -----------------
dw 0xAA55