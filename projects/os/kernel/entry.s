; global BIOSDiskReset
; global BIOSDiskRead
; global BIOSDiskWrite
; global BIOSGetDiskDriveParameters
global WriteByteToOutputPort
global ReadByteFromOutputPort

; %include "platforms\i686-elf\modes.s"
; %include "platforms\i686-elf\bios\disk.s"
%include "platforms\i686-elf\io\io.s"