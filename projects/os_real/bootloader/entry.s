bits 16

%include "projects\os_real\bootloader\entry.i"

%include "libraries\x86_real\bios\strings.s"
%include "libraries\x86_real\bios\disk.s"
%include "libraries\x86_real\math\integers.s"

; --------------------
; data
; --------------------
section .BootloaderEntryMessage
BootloaderEntryMessage:
    db 'Bootloader Entered...', CRLF, 0

section .DataSegmentAlignmentPadding
DataSegmentAlignmentPadding:
    db 0, 0, 0, 0, 0, 0, 0

; --------------------
; entry point
; --------------------
section .entry
entry:
    mov si, BootloaderEntryMessage
    call _BIOS_PrintString

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