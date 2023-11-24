; ----------------------
; switch to real mode code 
; from protected 32-bit code
; ----------------------
%macro x86EnterRealMode 0
    [bits 32]
    jmp word 18h:.ProtectedMode16Bits

.ProtectedMode16Bits:
    [bits 16]
    ; 2 - disable protected mode bit in cr0
    mov eax, cr0
    and al, ~1
    mov cr0, eax

    ; 3 - jump to real mode
    jmp word 00h:.RealModeCode

.RealModeCode:
    ; 4 - setup segments
    mov ax, 0
    mov ds, ax
    mov ss, ax

    ; 5 - enable interrupts
    sti
%endmacro

; ----------------------
; switch to protected 32-bit code 
; from real mode code
; ----------------------
%macro x86EnterProtectedMode 0
    cli

    ; 4 - set protection enable flag in CR0
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; 5 - far jump into protected mode
    jmp dword 08h:.ProtectedModeCode

.ProtectedModeCode:
    ; we are now in protected mode!
    [bits 32]

    ; 6 - setup segment registers
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
%endmacro

; -----------------------------------------
; Convert linear address to segment:offset address
; Args:
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