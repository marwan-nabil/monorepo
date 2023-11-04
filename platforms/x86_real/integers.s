; --------------------
; divides a u64 by a u32
; --------------------
section _TEXT._X86_DivideU64ByU32 class=CODE
_X86_DivideU64ByU32:
    ; make new call frame
    push bp ; save old call frame
    mov bp, sp ; initialize new call frame

    ; stack
    ; word [bp] -> previous bp
    ; word [bp + 2] -> return offset
    ; dword [bp + 4] -> lower 32 bits of divident
    ; dword [bp + 8] -> upper 32 bits of dividend
    ; dword [bp + 12] -> 32 bits of divisor
    ; word [bp + 16] -> quotient out pointer
    ; word [bp + 18] -> remainder out pointer

    push bx

    ; divide upper 32 bits
    mov eax, [bp + 8]
    mov ecx, [bp + 12]
    xor edx, edx
    div ecx ; eax -> quotient, edx -> remainder

    ; store upper 32 bits of quotient
    mov bx, [bp + 16]
    mov [bx + 4], eax

    ; divide lower 32 bits
    mov eax, [bp + 4]
    ; edx -> remainder from previous divide
    div ecx ; eax -> quotient, edx -> remainder

    ; store results
    mov [bx], eax
    mov bx, [bp + 18]
    mov [bx], edx

    pop bx

    ; restore old call frame
    mov sp, bp
    pop bp
    ret

; --------------------
; divides a u32 by u32
; in:
;       dx:ax --> dividend 
;       cx:bx --> divisor 
; out:
;       dx:ax --> quotient 
;       cx:bx --> remainder 
; --------------------
section _TEXT.__U4D class=CODE
__U4D:
    shl edx, 16
    mov dx, ax
    ; edx == dividend

    mov eax, edx
    xor edx, edx
    ; eax == dividend, edx == 0

    shl ecx, 16
    mov cx, bx
    ; ecx == divisor

    div ecx ; eax: quotient, edx: remainder

    mov ebx, edx
    mov ecx, edx
    shr ecx, 16

    mov edx, eax
    shr edx, 16

    ret

; --------------------------------------
; multiplies a u32 by a u32
; in:
;       DX:AX operand 1
;       CX:BX operand 2
; out:
;       DX:AX product
; side effects:
;       CX, BX modified
; --------------------------------------
section _TEXT.__U4M class=CODE
__U4M:
    shl edx, 16 ; dx to upper half of edx
    mov dx, ax ; m1 in edx
    mov eax, edx ; m1 in eax

    shl ecx, 16 ; cx to upper half of ecx
    mov cx, bx ; m2 in ecx

    mul ecx ; result in edx:eax (we only need eax)
    mov edx, eax ; move upper half to dx
    shr edx, 16

    ret