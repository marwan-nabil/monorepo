%define CRLF 0x0D, 0x0A

extern cstart

global entry

global BIOSPrintCharacter
global BIOS_PrintString
global BIOSDiskReset
global BIOSDiskRead
global BIOSDiskWrite
global BIOSGetDiskDriveParameters
global DivideU64ByU32

global BootloaderEntryMessage