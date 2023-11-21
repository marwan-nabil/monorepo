bits 16

%include "projects\os\bootloader\entry.i"

%include "libraries\x86\bios\strings.s"
%include "libraries\x86\bios\disk.s"
%include "libraries\x86\math\integers.s"

; --------------------
; data
; --------------------
section .data
BootloaderEntryMessage:
    db 'Bootloader Entered...', CRLF, 0

section .data
DataSegmentAlignmentPadding:
    db 0, 0, 0, 0, 0, 0, 0

; --------------------
; entry point
; --------------------
section .entry
entry:
    mov si, BootloaderEntryMessage
    call BIOSPrintString

    cli
    mov ax, ds
    mov ss, ax
    mov sp, 0
    mov bp, sp
    sti

    xor dh, dh
    push dx
    call cstart

    ; _cstart should not return
    cli
    hlt