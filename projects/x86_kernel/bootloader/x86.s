bits 16

%define CRLF 0x0D, 0x0A

extern _cstart

section _ENTRY class=CODE

; --------------------
; entry point
; --------------------
global entry
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
; message string
; --------------------
BootloaderEntryMessage:
    db 'Bootloader Entered.', CRLF, 0

section _TEXT class=CODE

; --------------------
; writes a character using a BIOS function
; --------------------
global _x86_PrintCharacter
_x86_PrintCharacter:
    push bp
    mov bp, sp
    push bx

    ; [bp + 0] == previous bp
    ; [bp + 2] == return address
    ; [bp + 4] == first argument: character to print
    ; [bp + 6] == second argument: page
    mov ah, 0x0E
    mov al, [bp + 4]
    mov bh, [bp + 6]

    int 0x10

    pop bx
    mov sp, bp
    pop bp
    ret

; --------------------
; divides a u64 by a u32
; --------------------
global _x86_Divide64By32
_x86_Divide64By32:
    ; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    push bx

    ; divide upper 32 bits
    mov eax, [bp + 8]   ; eax <- upper 32 bits of dividend
    mov ecx, [bp + 12]  ; ecx <- divisor
    xor edx, edx
    div ecx             ; eax - quot, edx - remainder

    ; store upper 32 bits of quotient
    mov bx, [bp + 16]
    mov [bx + 4], eax

    ; divide lower 32 bits
    mov eax, [bp + 4]   ; eax <- lower 32 bits of dividend
                        ; edx <- old remainder
    div ecx

    ; store results
    mov [bx], eax
    mov bx, [bp + 18]
    mov [bx], edx

    pop bx

    ; restore old call frame
    mov sp, bp
    pop bp
    ret

;--------------------------------------------
; does an unsigned 4 byte divide
; Input:
;       DX:AX -> Dividend
;       CX:BX -> Divisor
; Output:
;       DX:AX -> Quotient
;       CX:BX -> Remainder
;--------------------------------------------
global __U4D
__U4D:
    shl edx, 16         ; dx to upper half of edx
    mov dx, ax          ; edx - dividend
    mov eax, edx        ; eax - dividend
    xor edx, edx

    shl ecx, 16         ; cx to upper half of ecx
    mov cx, bx          ; ecx - divisor

    div ecx             ; eax - quot, edx - remainder
    mov ebx, edx
    mov ecx, edx
    shr ecx, 16

    mov edx, eax
    shr edx, 16

    ret

;--------------------------------------------
; integer four byte multiply
; Input:
;       DX:AX -> integer 1
;       CX:BX -> integer 2
; Output:
;       DX:AX -> product
;       CX, BX -> modified
;--------------------------------------------
global __U4M
__U4M:
    shl edx, 16         ; dx to upper half of edx
    mov dx, ax          ; m1 in edx
    mov eax, edx        ; m1 in eax

    shl ecx, 16         ; cx to upper half of ecx
    mov cx, bx          ; m2 in ecx

    mul ecx             ; result in edx:eax (we only need eax)
    mov edx, eax        ; move upper half to dx
    shr edx, 16

    ret