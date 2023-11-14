%define CRLF 0x0D, 0x0A

extern _cstart

global entry

global _BIOS_PrintCharacter

global _DivideU64ByU32
global __U4D
global __U4M

global _BIOS_DiskReset
global _BIOS_DiskRead
global _BIOS_DiskWrite
global _BIOS_GetDiskDriveParameters