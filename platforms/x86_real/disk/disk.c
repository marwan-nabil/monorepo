b8 GetDiskDriveParameters(disk_parameters *DiskParameters, u8 DriveNumber)
{
    b8 Result = FALSE;
    u8 DriveType;
    u16 Cylinders;
    u16 Sectors;
    u16 Heads;

    DiskParameters->Id = DriveNumber;
    Result = X86_GetDiskDriveParameters(DriveNumber, &DriveType, &Cylinders, &Sectors, &Heads);
    if (!Result)
    {
        return FALSE;
    }

    DiskParameters->Cylinders = Cylinders;
    DiskParameters->Heads = Heads;
    DiskParameters->Sectors = Sectors;

    return TRUE;
}

void TranslateLbaToChs
(
    disk_parameters *DiskParameters,
    u32 LogicalBlockAddress, u16 *Cylinder, u16 *Head, u16 *Sector
)
{
    *Cylinder = (LogicalBlockAddress / DiskParameters->Sectors) / DiskParameters->Heads;
    *Head = (LogicalBlockAddress / DiskParameters->Sectors) % DiskParameters->Heads;
    *Sector = (LogicalBlockAddress % DiskParameters->Sectors) + 1;
}

b8 ReadDiskSectors
(
    disk_parameters *DiskParameters,
    u32 LogicalBlockAddress, u8 SectorsToRead, u8 far *DataOut
)
{
    u16 Cylinder, Head, Sector;
    TranslateLbaToChs(DiskParameters, LogicalBlockAddress, &Cylinder, &Head, &Sector);

    for (u16 Retry = 0; Retry < DISK_OPERATION_MAXIMUM_RETRIES; Retry++)
    {
        b8 ReadOk = X86_DiskRead
        (
            DiskParameters->Id,
            Cylinder, Head, Sector,
            SectorsToRead, DataOut
        );

        if (ReadOk)
        {
            return TRUE;
        }
        else
        {
            X86_DiskReset(DiskParameters->Id);
        }
    }

    return FALSE;
}