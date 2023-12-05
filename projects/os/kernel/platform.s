; %include "libraries\i686-elf\cpu\modes.s"
%include "libraries\i686-elf\cpu\io.s"
%include "libraries\i686-elf\cpu\gdt.s"
%include "libraries\i686-elf\cpu\idt.s"
%include "libraries\i686-elf\cpu\panic.s"
; %include "libraries\i686-elf\bios\disk.s"

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