; global BIOSDiskReset
; global BIOSDiskRead
; global BIOSDiskWrite
; global BIOSGetDiskDriveParameters
global WriteByteToOutputPort
global ReadByteFromOutputPort

; %include "libraries\x86\modes.s"
; %include "libraries\x86\bios\disk.s"
%include "libraries\x86\io\io.s"