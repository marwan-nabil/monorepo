section .text
LoadIDT:
    [bits 32]
    push ebp
    mov ebp, esp
    
    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret