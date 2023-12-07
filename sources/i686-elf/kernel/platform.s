; %include "shared\cpu\modes.s"
%include "shared\cpu\io.s"
%include "shared\cpu\gdt.s"
%include "shared\cpu\idt.s"
%include "shared\cpu\panic.s"
; %include "shared\bios\disk.s"

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