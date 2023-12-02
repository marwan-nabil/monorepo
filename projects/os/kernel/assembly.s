; global BIOSDiskReset
; global BIOSDiskRead
; global BIOSDiskWrite
; global BIOSGetDiskDriveParameters
global WriteByteToOutputPort
global ReadByteFromOutputPort
global LoadGDT
global LoadIDT

%include "platforms\i686-elf\cpu\modes.s"
%include "platforms\i686-elf\cpu\io.s"
%include "platforms\i686-elf\cpu\gdt.s"
%include "platforms\i686-elf\cpu\idt.s"
; %include "platforms\i686-elf\bios\disk.s"