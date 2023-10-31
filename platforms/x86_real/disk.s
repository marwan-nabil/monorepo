; ---------------------------------------
; resets the floppy disk
; in:
;       word [sp + 2] == driver number
; ---------------------------------------
section _TEXT._X86_DiskReset class=CODE
_X86_DiskReset:
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

; global _X86_DiskRead
; _X86_DiskRead:
;     push bp
;     mov bp, sp

;     mov dl, [bp + 4] ; drive number
;     mov ah, 0
;     stc
;     int 0x13

;     mov ax, 1
;     ; carry flag is 1 in case of error in the BIOS function
;     sbb ax, 0 ; subtract the carry flag from ax

;     mov sp, bp
;     pop bp
;     ret


; global _X86_GetDiskDriveParameters
; _X86_GetDiskDriveParameters: