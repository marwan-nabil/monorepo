; ----------------------
; switch to real mode code 
; from protected 32-bit code
; ----------------------
%macro x86EnterRealMode 0
    [bits 32]
    jmp word 18h:.ProtectedMode16Bits

.ProtectedMode16Bits:
    [bits 16]
    mov eax, cr0
    and al, ~1
    mov cr0, eax
    jmp word 00h:.RealModeCode

.RealModeCode:
    mov ax, 0
    mov ds, ax
    mov ss, ax
    sti
%endmacro

; ----------------------
; switch to protected 32-bit code 
; from real mode code
; ----------------------
%macro x86EnterProtectedMode 0
    cli
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp dword 08h:.ProtectedModeCode

.ProtectedModeCode:
    [bits 32]
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
%endmacro

; -----------------------------------------
; Convert linear address to segment:offset address
; arguments:
;    1: linear address
;    2: (out) target segment
;    3: target 32-bit register to use
;    4: target lower 16-bit half of #3
; -----------------------------------------
%macro ConvertLinearAddressToSegmentOffsetAddress 4
    mov %3, %1
    shr %3, 4
    mov %2, %4
    mov %3, %1
    and %3, 0xf
%endmacro