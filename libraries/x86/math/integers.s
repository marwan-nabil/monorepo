; --------------------
; divides a u64 by a u32
; --------------------
section .text
DivideU64ByU32:
    [bits 32]
    push ebp
    mov ebp, esp

    ; stack
    ; word [bp] -> previous bp
    ; word [bp + 4] -> return offset
    ; dword [bp + 8] -> lower 32 bits of dividend
    ; dword [bp + 12] -> upper 32 bits of dividend
    ; dword [bp + 16] -> 32 bits of divisor
    ; word [bp + 20] -> quotient out pointer
    ; word [bp + 24] -> remainder out pointer

    push ebx

    ; divide upper 32 bits
    mov eax, [bp + 12]
    mov ecx, [bp + 16]
    xor edx, edx
    div ecx ; eax -> quotient, edx -> remainder

    ; store upper 32 bits of quotient
    mov ebx, [ebp + 20]
    mov [ebx + 4], eax

    ; divide lower 32 bits
    mov eax, [ebp + 8]
    ; edx -> remainder from previous divide
    div ecx ; eax -> quotient, edx -> remainder

    ; store results
    mov [ebx], eax
    mov ebx, [ebp + 24]
    mov [ebx], edx

    pop ebx

    mov esp, ebp
    pop ebp
    ret