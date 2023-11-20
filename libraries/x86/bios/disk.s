; ---------------------------------------
; resets the floppy disk
; in:
;       word [sp + 2] == driver number
; ---------------------------------------
section .text
BIOSDiskReset:
    push bp
    mov bp, sp

    mov dl, [bp + 4] ; drive number
    mov ah, 0
    stc
    int 0x13

    mov ax, 1
    ; carry flag is 1 in case of error in the BIOS function
    sbb ax, 0 ; subtract the carry flag from ax

    mov sp, bp
    pop bp
    ret

; ---------------------------------------
; reads sectors from floppy disk given the
; CHS and the sector count
; void __attribute__((cdecl)) BIOSDiskRead
; (
;     u8 DriveNumber, u16 Cylinder, u16 Head,
;     u16 Sector, u8 SectorCount, u8 *DataOut
; );
; ---------------------------------------
section .text
BIOSDiskRead:
    push bp
    mov bp, sp

    ; save touched registers
    push bx
    push es

    ; setup arguments for the BIOS
    ; read disk service
    mov dl, [bp + 4] ; drive number

    mov ch, [bp + 6] ; cylinder[7:0]
    mov cl, [bp + 7] ; cylinder[15:8]
    shl cl, 6

    mov dh, [bp + 8] ; head

    mov al, [bp + 10] ; sector
    and al, 0x3F
    or cl, al ; cl == {cylinder[9:8], sector[5:0]}

    mov al, [bp + 12] ; SectorCount

    mov bx, [bp + 16] ; DataOut [31:16]
    mov es, bx
    mov bx, [bp + 14] ; DataOut [15:0]

    mov ah, 0x02 ; disk service for reading
    stc
    int 0x13

    mov ax, 1
    ; carry flag is 1 in case of error in the BIOS function
    sbb ax, 0 ; subtract the carry flag from ax

    ; restore touched registers
    pop es
    pop bx

    mov sp, bp
    pop bp
    ret

; ---------------------------------------
; writes sectors to floppy disk given the
; CHS and the sector count
; void __attribute__((cdecl)) BIOSDiskWrite
; (
;     u8 DriveNumber, u16 Cylinder, u16 Head,
;     u16 Sector, u8 SectorCount, u8 *DataIn
; );
; ---------------------------------------
section .text
BIOSDiskWrite:
    push bp
    mov bp, sp

    ; save touched registers
    push bx
    push es

    ; setup arguments for the BIOS
    ; read disk service
    mov dl, [bp + 4] ; drive number

    mov ch, [bp + 6] ; cylinder[7:0]
    mov cl, [bp + 7] ; cylinder[15:8]
    shl cl, 6

    mov dh, [bp + 8] ; head

    mov al, [bp + 10] ; sector
    and al, 0x3F
    or cl, al ; cl == {cylinder[9:8], sector[5:0]}

    mov al, [bp + 12] ; SectorCount

    mov bx, [bp + 16] ; DataIn [31:16]
    mov es, bx
    mov bx, [bp + 14] ; DataIn [15:0]

    mov ah, 0x03 ; disk service for reading
    stc
    int 0x13

    mov ax, 1
    ; carry flag is 1 in case of error in the BIOS function
    sbb ax, 0 ; subtract the carry flag from ax

    ; restore touched registers
    pop es
    pop bx

    mov sp, bp
    pop bp
    ret

; ---------------------------------------
;
; void __attribute__((cdecl)) BIOSGetDiskDriveParameters
; (
;     u8 DriveNumber, u8 *DriveTypeOut, u16 *CylindersOut,
;     u16 *SectorsOut, u16 *HeadsOut
; );
; ---------------------------------------
section .text
BIOSGetDiskDriveParameters:
    push bp
    mov bp, sp

    ; save registers
    push es
    push bx
    push si
    push di

    ; prepare arguments for BIOS service that
    ; gets the drive parameters
    mov dl, [bp + 4] ; drive number
    mov ah, 0x08 ; BIOS service number
    ; es:di == 0 to avoid BIOS bugs
    mov di, 0
    mov es, di
    stc
    int 0x13

    mov ax, 1
    sbb ax, 0

    ; store results of BIOS service
    mov si, [bp + 6]
    mov [si], bl ; drive type

    mov bl, ch ; maximum cylinder number [7:0]
    mov bh, cl ; maximum cylinder number [9:8]
    shr bh, 6
    mov si, [bp + 8]
    mov [si], bx ; bx[9:0] == maximum number of cylinders

    xor ch, ch
    and cl, 0x3F
    mov si, [bp + 10]
    mov [si], cx ; cx[5:0] == maximum sector number

    mov cl, dh
    mov si, [bp + 12]
    mov [si], cx ; cx == maximum number of heads

    ; restore registers
    pop di
    pop si
    pop bx
    pop es

    mov sp, bp
    pop bp
    ret