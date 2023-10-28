#pragma once

typedef struct
{
    u8 Id;
    u16 Cylinders;
    u16 Sectors;
    u16 Heads;
} floppy_disk_parameters;

void _cdecl X86_DiskReset(u8 DriveNumber);
void _cdecl X86_DiskRead
(
    u8 DriveNumber, u16 Cylinder, u16 Head,
    u16 Sector, u8 Count, u8 far *DataOut
);

void _cdecl X86_GetDiskDriveParameters
(
    u8 DriveNumber, u8 *DriveTypeOut, u16 *CylindersOut,
    u16 *SectorsOut, u16 *HeadsOut
);