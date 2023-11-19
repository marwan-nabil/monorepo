%define CRLF 0x0D, 0x0A

extern cstart

global entry

global BIOS_PrintCharacter
global BIOS_PrintString
global BIOS_DiskReset
global BIOS_DiskRead
global BIOS_DiskWrite
global BIOS_GetDiskDriveParameters
global DivideU64ByU32

global BootloaderEntryMessage