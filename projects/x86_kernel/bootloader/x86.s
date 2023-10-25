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
    call _X86_PrintString

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
; message string
; --------------------
BootloaderEntryMessage:
    db 'Bootloader Entered.', CRLF, 0

section _TEXT class=CODE

%include "platforms\x86_real\strings.s"
%include "platforms\x86_real\integers.s"