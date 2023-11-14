void GetDiskDriveParameters(disk_parameters far *DiskParameters, u8 DriveNumber)
{
    b8 Result = FALSE;
    u8 DriveType;
    u16 Cylinders;
    u16 Sectors;
    u16 Heads;

    DiskParameters->Id = DriveNumber;
    Result = BIOS_GetDiskDriveParameters(DriveNumber, &DriveType, &Cylinders, &Sectors, &Heads);
    if (!Result)
    {
        PrintFormatted("ERROR: GetDiskDriveParameters() failed.\r\n");
        return;
    }

    DiskParameters->Cylinders = Cylinders + 1;
    DiskParameters->Heads = Heads + 1;
    DiskParameters->Sectors = Sectors;
    DiskParameters->Type = DriveType;
}

void TranslateLbaToChs
(
    disk_parameters far *DiskParameters,
    u32 LogicalBlockAddress, u16 *Cylinder, u16 *Head, u16 *Sector
)
{
    *Cylinder = (LogicalBlockAddress / DiskParameters->Sectors) / DiskParameters->Heads;
    *Head = (LogicalBlockAddress / DiskParameters->Sectors) % DiskParameters->Heads;
    *Sector = (LogicalBlockAddress % DiskParameters->Sectors) + 1;
}

void ReadDiskSectors
(
    disk_parameters far *DiskParameters,
    u32 LogicalBlockAddress,
    u8 SectorsToRead,
    void far *DataOut
)
{
    u16 Cylinder, Head, Sector;
    TranslateLbaToChs(DiskParameters, LogicalBlockAddress, &Cylinder, &Head, &Sector);

    for (u16 Retry = 0; Retry < DISK_OPERATION_MAXIMUM_RETRIES; Retry++)
    {
        b8 ReadOk = BIOS_DiskRead
        (
            DiskParameters->Id,
            Cylinder, Head, Sector,
            SectorsToRead, DataOut
        );

        if (ReadOk)
        {
            return;
        }
        else
        {
            BIOS_DiskReset(DiskParameters->Id);
        }
    }

    PrintFormatted("ERROR: ReadDiskSectors() failed.\r\n");
}

void WriteDiskSectors
(
    disk_parameters far *DiskParameters,
    u32 LogicalBlockAddress,
    u8 SectorsToWrite,
    u8 far *DataIn
)
{
    u16 Cylinder, Head, Sector;
    TranslateLbaToChs(DiskParameters, LogicalBlockAddress, &Cylinder, &Head, &Sector);

    for (u16 Retry = 0; Retry < DISK_OPERATION_MAXIMUM_RETRIES; Retry++)
    {
        b8 WriteOk = BIOS_DiskWrite
        (
            DiskParameters->Id,
            Cylinder, Head, Sector,
            SectorsToWrite, DataIn
        );

        if (WriteOk)
        {
            return;
        }
        else
        {
            BIOS_DiskReset(DiskParameters->Id);
        }
    }

    PrintFormatted("ERROR: WriteDiskSectors() failed.\r\n");
}