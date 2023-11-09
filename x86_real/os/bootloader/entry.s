bits 16

%include "x86_real\os\bootloader\entry.hs"

%include "x86_real\shared\strings\strings.s"
%include "x86_real\shared\math\integers.s"
%include "x86_real\shared\disk\disk.s"

; --------------------
; data
; --------------------
section _ENTRY.BootloaderEntryMessage class=CODE
BootloaderEntryMessage:
    db 'Bootloader Entered...', CRLF, 0

section _DATA.DataSegmentAlignmentPadding class=DATA
DataSegmentAlignmentPadding:
    db 0

; --------------------
; entry point
; --------------------
section _ENTRY.entry class=CODE
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