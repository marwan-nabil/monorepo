#pragma once

#define DISK_OPERATION_MAXIMUM_RETRIES 3

typedef struct
{
    u8 Id;
    u16 Cylinders;
    u16 Heads;
    u16 Sectors;
} disk_parameters;

b8 _cdecl X86_DiskReset(u8 DriveNumber);

b8 _cdecl X86_DiskRead
(
    u8 DriveNumber, u16 Cylinder, u16 Head,
    u16 Sector, u8 SectorCount, u8 far *DataOut
);

b8 _cdecl X86_GetDiskDriveParameters
(
    u8 DriveNumber, u8 *DriveTypeOut, u16 *CylindersOut,
    u16 *SectorsOut, u16 *HeadsOut
);