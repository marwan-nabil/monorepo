bits 16

%include "projects\x86_os\bootloader\entry.hs"

%include "platforms\x86_real\strings\strings.s"
%include "platforms\x86_real\math\integers.s"
%include "platforms\x86_real\disk\disk.s"

; --------------------
; data
; --------------------
section _ENTRY.BootloaderEntryMessage class=CODE
BootloaderEntryMessage:
    db 'Bootloader Entered...', CRLF, 0

section _DATA.DataSegmentAlignmentPadding class=DATA
DataSegmentAlignmentPadding:
    db 0,

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