b8 _cdecl InitializeDisk(floppy_disk_parameters *DiskParameters, u8 DriveNumber)
{
    return FALSE;
}

b8 _cdecl ReadDiskSectors
(
    floppy_disk_parameters *DiskParameters,
    u32 LogicalBlockAddress, u8 Sectors, u8 far *DataOut
)
{
    return FALSE;
}