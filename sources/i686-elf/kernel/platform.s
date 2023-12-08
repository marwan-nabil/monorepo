; %include "sources\i686-elf\shared\cpu\modes.s"
%include "sources\i686-elf\shared\cpu\io.s"
%include "sources\i686-elf\shared\cpu\gdt.s"
%include "sources\i686-elf\shared\cpu\idt.s"
%include "sources\i686-elf\shared\cpu\panic.s"
; %include "sources\i686-elf\shared\bios\disk.s"

; global BIOSDiskReset
; global BIOSDiskRead
; global BIOSDiskWrite
; global BIOSGetDiskDriveParameters
global WriteByteToOutputPort
global ReadByteFromOutputPort
global LoadGDT
global LoadIDT
global Panic
global IntentionalCrash