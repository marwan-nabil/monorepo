global Entry

; --------------------
; definitions
; --------------------
%define CRLF 0x0D, 0x0A

%define KEYBOARD_CONTROLLER_DATA_PORT 0x60
%define KEYBOARD_CONTROLLER_COMMAND_PORT 0x64

%define KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD 0xAD
%define KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD 0xAE
%define KEYBOARD_CONTROLLER_COMMAND_READ 0xD0
%define KEYBOARD_CONTROLLER_COMMAND_WRITE 0xD1

%define VGA_SCREEN_BUFFER 0xB8000

; ---------------------
; real mode entry point
; ---------------------
[bits 16]
section .entry
Entry:
    ; initialize segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; initialize stack
    mov sp, 0x7C00

    ; swtich to 32 bit protected mode
    cli
    call EnableA20Line
    call SetupGDT
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp dword 0x08:ProtectedModeEntryPoint

; --------------------------
; protected mode entry point
; --------------------------
[bits 32]
section .text
ProtectedModeEntryPoint:
    ; setup segment registers
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

    ; print hello world to VGA display
    mov esi, HelloWorldStringProtected
    mov edi, VGA_SCREEN_BUFFER
    cld
    mov bl, 1

.Loop:
    lodsb
    or al, al
    jz .Done

    ; write character
    mov [edi], al
    inc edi

    ; write color
    mov [edi], bl
    inc bl
    and bl, 0x0F
    jnz .NotBlackColor
    or bl, 0x01

.NotBlackColor:
    inc edi
    jmp .Loop

.Done:
    ; swtich to 16 bit protected mode
    jmp word 0x18:ProtectedMode16BitCode

[bits 16]
ProtectedMode16BitCode:
    ; disable protected mode
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax

    ; jump to real mode code
    jmp word 0x00:RealModeCode

[bits 16]
RealModeCode:
    ; setup the segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    sti

    ; test BIOS services to make sure we're in real mode
    mov si, HelloWorldStringReal

.Loop:
    lodsb
    or al, al
    jz .Done
    mov ah, 0x0E
    int 0x10
    jmp .Loop

.Done:
.Halt:
    jmp .Halt

; ----------------------------
; enables the A20 address line
; ----------------------------
[bits 16]
section .text
EnableA20Line:
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
[bits 16]
section .text
WaitUntilKeyboardControllerCanBeWritten:
    in al, KEYBOARD_CONTROLLER_COMMAND_PORT
    test al, 2
    jnz WaitUntilKeyboardControllerCanBeWritten
    ret

; ----------------------------
; wait until keyboard controller status bit 1 is cleared
; status bit 2 is for the controller output buffer
; ----------------------------
[bits 16]
section .text
WaitUntilKeyboardControllerCanBeRead:
    in al, KEYBOARD_CONTROLLER_COMMAND_PORT
    test al, 1
    jz WaitUntilKeyboardControllerCanBeRead
    ret

; ----------------------------
; sets up the global descriptor table
; for protected mode segment access
; ----------------------------
[bits 16]
section .text
SetupGDT:
    lgdt [GlobalDescriptorTableDescriptor]
    ret

; ================================================================= ;
;                              data
; ================================================================= ;
section .data
GlobalDescriptorTable:
    dq 0

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

section .data
GlobalDescriptorTableDescriptor:
    dw GlobalDescriptorTableDescriptor - GlobalDescriptorTable - 1
    dd GlobalDescriptorTable

section .data
HelloWorldStringProtected:
    db "Hello world, from protected mode", 0

section .data
HelloWorldStringReal:
    db "Hello world, from real mode", 0
