#pragma once

b8 __attribute__((cdecl)) BIOS_DiskReset(u8 DriveNumber);

b8 __attribute__((cdecl)) BIOS_DiskRead
(
    u8 DriveNumber, u16 Cylinder, u16 Head,
    u16 Sector, u8 SectorCount, u8 *DataOut
);

b8 __attribute__((cdecl)) BIOS_DiskWrite
(
    u8 DriveNumber, u16 Cylinder, u16 Head,
    u16 Sector, u8 SectorCount, u8 *DataIn
);

b8 __attribute__((cdecl)) BIOS_GetDiskDriveParameters
(
    u8 DriveNumber, u8 *DriveTypeOut, u16 *CylindersOut,
    u16 *SectorsOut, u16 *HeadsOut
);