org 0x7C00

%define CRLF 0x0D, 0x0A

%define KEYBOARD_CONTROLLER_DATA_PORT 0x60
%define KEYBOARD_CONTROLLER_COMMAND_PORT 0x64

%define KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD 0xAD
%define KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD 0xAE
%define KEYBOARD_CONTROLLER_COMMAND_READ 0xD0
%define KEYBOARD_CONTROLLER_COMMAND_WRITE 0xD1

; ================================================================= ;
;                       boot sector code
; ================================================================= ;
; --------------------
; entry point
; --------------------
Entry:
    [bits 16]
    ; initialize segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; initialize stack
    mov sp, 0x7C00

    ; ------------------------
    ; swtich to protected mode
    ; ------------------------
    cli
    call EnableA20Line
    call SetupGDT
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dword 0x08:.ProtectedModeCode

.ProtectedModeCode:
    [bits 32]
    mov ah, 0x0e
    mov al, 'H'
    int 0x10

.Halt:
    jmp .Halt

; ----------------------------
; enables the A20 address line
; ----------------------------
EnableA20Line:
    [bits 16]
    call WaitUntilKeyboardControllerCanBeWritten
    mov al, KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD
    out KEYBOARD_CONTROLLER_COMMAND_PORT, al

    call WaitUntilKeyboardControllerCanBeWritten
    mov al, KEYBOARD_CONTROLLER_COMMAND_READ
    out KEYBOARD_CONTROLLER_COMMAND_PORT, al

    call WaitUntilKeyboardControllerCanBeRead
    in al, KEYBOARD_CONTROLLER_DATA_PORT
    push eax

    call WaitUntilKeyboardControllerCanBeWritten
    mov al, KEYBOARD_CONTROLLER_COMMAND_WRITE
    out KEYBOARD_CONTROLLER_COMMAND_PORT, al

    call WaitUntilKeyboardControllerCanBeWritten
    pop eax
    or al, 2 ; set the A20 line bit
    out KEYBOARD_CONTROLLER_DATA_PORT, al

    call WaitUntilKeyboardControllerCanBeWritten
    mov al, KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD
    out KEYBOARD_CONTROLLER_COMMAND_PORT, al

    call WaitUntilKeyboardControllerCanBeWritten
    ret

; ----------------------------
; wait until keyboard controller status bit 2 is cleared
; status bit 2 is for the controller input buffer
; ----------------------------
WaitUntilKeyboardControllerCanBeWritten:
    [bits 16]
    in al, KEYBOARD_CONTROLLER_COMMAND_PORT
    test al, 2
    jnz WaitUntilKeyboardControllerCanBeWritten
    ret

; ----------------------------
; wait until keyboard controller status bit 1 is cleared
; status bit 2 is for the controller output buffer
; ----------------------------
WaitUntilKeyboardControllerCanBeRead:
    [bits 16]
    in al, KEYBOARD_CONTROLLER_COMMAND_PORT
    test al, 1
    jnz WaitUntilKeyboardControllerCanBeRead
    ret

; ----------------------------
; sets up the global descriptor table
; for protected mode segment access
; ----------------------------
SetupGDT:
    [bits 16]
    lgdt [GlobalDescriptorTableDescriptor]
    ret

; ================================================================= ;
;                       boot sector data
; ================================================================= ;
GlobalDescriptorTable:
    ; dq 0

    ; 32-bit code segment
    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
    dw 0                        ; base (bits 0-15) = 0x0
    db 0                        ; base (bits 16-23)
    db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
    db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
    db 0                        ; base high

    ; 32-bit data segment
    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
    dw 0                        ; base (bits 0-15) = 0x0
    db 0                        ; base (bits 16-23)
    db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
    db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
    db 0                        ; base high

    ; 16-bit code segment
    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
    dw 0                        ; base (bits 0-15) = 0x0
    db 0                        ; base (bits 16-23)
    db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
    db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
    db 0                        ; base high

    ; 16-bit data segment
    dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF
    dw 0                        ; base (bits 0-15) = 0x0
    db 0                        ; base (bits 16-23)
    db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
    db 00001111b                ; granularity (1b pages, 16-bit pmode) + limit (bits 16-19)
    db 0                        ; base high

GlobalDescriptorTableDescriptor:
    dw GlobalDescriptorTableDescriptor - GlobalDescriptorTable - 1
    dd GlobalDescriptorTable

; ---------------------------------------
; pad with 0 until you reach address 0x7DFE
; ---------------------------------------
times 510 - ($ - $$) db 0

; -----------------
; 0x7DFE, boot sector signature, 2 bytes
; -----------------
dw 0xAA55