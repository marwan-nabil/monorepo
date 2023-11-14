b8 IsFatEntryEndOfFile(u16 FatEntry)
{
    if
    (
        (FatEntry >= FAT12_FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_START) &&
        (FatEntry <= FAT12_FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END)
    )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

u16 TranslateFatClusterNumberToSectorIndex(u16 ClusterNumber)
{
    u16 PhysicalSectorIndex = FAT12_DATA_AREA_START_SECTOR + ClusterNumber - 2;
    return PhysicalSectorIndex;
}

u16 TranslateSectorIndexToFatClusterNumber(u16 SectorIndex)
{
    u16 ClusterNumber = SectorIndex + 2 - FAT12_DATA_AREA_START_SECTOR;
    return ClusterNumber;
}

void GetDiskSectorFromFatClusterNumber
(
    disk_parameters far *DiskParameters,
    sector far *SectorLocation,
    u16 ClusterNumber
)
{
    u16 LBA = TranslateFatClusterNumberToSectorIndex(ClusterNumber);
    ReadDiskSectors(DiskParameters, LBA, 1, (u8 far *)SectorLocation);
}

u16 GetFatEntryFromClusterNumber(fat12_ram_disk far *Disk, u32 ClusterNumber)
{
    u16 Result = 0;
    u32 StartingByteIndex = ClusterNumber * 3 / 2;

    if ((ClusterNumber % 2) == 0)
    {
        Result = (u16)Disk->Fat.Bytes[StartingByteIndex];
        Result |= ((u16)Disk->Fat.Bytes[StartingByteIndex + 1] & 0x000F) << 8;
    }
    else
    {
        Result = (u16)(Disk->Fat.Bytes[StartingByteIndex] & 0xF0) >> 4;
        Result |= ((u16)Disk->Fat.Bytes[StartingByteIndex + 1]) << 4;
    }

    return Result;
}

u16 GetFirstFreeClusterNumber(fat12_ram_disk far *Disk)
{
    for (u16 ClusterNumber = 2; ClusterNumber < FAT12_FAT_ENTRIES_IN_FAT; ClusterNumber++)
    {
        u16 FatEntry = GetFatEntryFromClusterNumber(Disk, ClusterNumber);
        if (FatEntry == FAT12_FAT_ENTRY_FREE_CLUSTER)
        {
            return ClusterNumber;
        }
    }
    return 0;
}

u32 CalculateFreeClusterNumbers(fat12_ram_disk far *Disk)
{
    u32 Result = 0;

    for (u16 ClusterNumber = 2; ClusterNumber < FAT12_FAT_ENTRIES_IN_FAT; ClusterNumber++)
    {
        u16 FatEntry = GetFatEntryFromClusterNumber(Disk, ClusterNumber);
        if (FatEntry == FAT12_FAT_ENTRY_FREE_CLUSTER)
        {
            Result++;
        }
    }

    return Result;
}

directory_entry far *
GetFirstFreeDirectoryEntryInSector(sector far *Sector)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < FAT12_DIRECTORY_ENTRIES_IN_SECTOR;
        DirectoryEntryIndex++
    )
    {
        directory_entry far *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];
        if (DirectoryEntry->FileName[0] == FAT12_FILENAME_EMPTY_SLOT)
        {
            return DirectoryEntry;
        }
    }

    return NULL;
}

directory_entry far *
GetFirstFreeDirectoryEntryInRootDirectory(fat12_ram_disk far *Disk)
{
    directory_entry far *FirstFreeDirectoryEntry = NULL;

    for (u32 SectorIndex = 0; SectorIndex < FAT12_SECTORS_IN_ROOT_DIRECTORY; SectorIndex++)
    {
        FirstFreeDirectoryEntry =
            GetFirstFreeDirectoryEntryInSector(&Disk->RootDirectory.Sectors[SectorIndex]);

        if (FirstFreeDirectoryEntry)
        {
            break;
        }
    }

    return FirstFreeDirectoryEntry;
}

directory_entry far *
GetDirectoryEntryOfFileInSector(sector far *Sector, char far *FileName, char far *Extension)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < ArrayCount(Sector->DirectoryEntries);
        DirectoryEntryIndex++
    )
    {
        directory_entry far *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];
        if
        (
            (MemoryCompareFarToFar(DirectoryEntry->FileName, FileName, 8) == 0) &&
            (MemoryCompareFarToFar(DirectoryEntry->FileExtension, Extension, 3) == 0)
        )
        {
            return DirectoryEntry;
        }
    }

    return NULL;
}

directory_entry far *
GetDirectoryEntryOfDirectoryInSector(sector far *Sector, char far *DirectoryName)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < FAT12_DIRECTORY_ENTRIES_IN_SECTOR;
        DirectoryEntryIndex++
    )
    {
        directory_entry far *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];

        if
        (
            StringCompareFarToFar
            (
                DirectoryEntry->FileName,
                DirectoryName,
                StringLengthFar(DirectoryName)
            )
            == 
            0
        )
        {
            return DirectoryEntry;
        }
    }

    return NULL;
}

directory_entry far *
GetDirectoryEntryOfFileInDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    directory_entry far *Directory,
    char far *FileName,
    char far *Extension
)
{
    directory_entry far *FoundDirectoryEntry = NULL;

    u16 CurrentClusterNumber = Directory->ClusterNumberLowWord;
    u16 CurrentFatEntry = GetFatEntryFromClusterNumber(Disk, CurrentClusterNumber);

    for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
    {
        sector far *PhysicalSector = PushStruct(MemoryArena, sector);
        GetDiskSectorFromFatClusterNumber
        (
            DiskParameters,
            PhysicalSector,
            CurrentClusterNumber
        );
        FoundDirectoryEntry = GetDirectoryEntryOfFileInSector(PhysicalSector, FileName, Extension);

        if (FoundDirectoryEntry)
        {
            break;
        }
        else
        {
            if (IsFatEntryEndOfFile(CurrentFatEntry))
            {
                break;
            }
            else
            {
                CurrentClusterNumber = CurrentFatEntry;
                CurrentFatEntry = GetFatEntryFromClusterNumber(Disk, CurrentClusterNumber);
            }
        }
    }

    return FoundDirectoryEntry;
}

directory_entry far *
GetDirectoryEntryOfDirectoryInDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    directory_entry far *Directory,
    char far *DirectoryName
)
{
    directory_entry far *FoundDirectoryEntry = NULL;

    u16 CurrentClusterNumber = Directory->ClusterNumberLowWord;
    u16 CurrentFatEntry = GetFatEntryFromClusterNumber(Disk, CurrentClusterNumber);

    for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
    {
        sector far *PhysicalSector = PushStruct(MemoryArena, sector);
        GetDiskSectorFromFatClusterNumber
        (
            DiskParameters,
            PhysicalSector,
            CurrentClusterNumber
        );
        FoundDirectoryEntry = GetDirectoryEntryOfDirectoryInSector(PhysicalSector, DirectoryName);

        if (FoundDirectoryEntry)
        {
            break;
        }
        else
        {
            if (IsFatEntryEndOfFile(CurrentFatEntry))
            {
                break;
            }
            else
            {
                CurrentClusterNumber = CurrentFatEntry;
                CurrentFatEntry = GetFatEntryFromClusterNumber(Disk, CurrentClusterNumber);
            }
        }
    }

    return FoundDirectoryEntry;
}

directory_entry far *
GetDirectoryEntryOfFileInRootDirectory
(
    fat12_ram_disk far *Disk,
    char far *FileName,
    char far *Extension
)
{
    directory_entry far *FoundDirectoryEntry = NULL;

    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
    {
        FoundDirectoryEntry =
            GetDirectoryEntryOfFileInSector(&Disk->RootDirectory.Sectors[SectorIndex], FileName, Extension);

        if (FoundDirectoryEntry)
        {
            break;
        }
    }

    return FoundDirectoryEntry;
}

directory_entry far *
GetDirectoryEntryOfDirectoryInRootDirectory
(
    fat12_ram_disk far *Disk,
    char far *DirectoryName
)
{
    directory_entry far *FoundDirectoryEntry = NULL;

    for (u32 SectorIndex = 0; SectorIndex < FAT12_SECTORS_IN_ROOT_DIRECTORY; SectorIndex++)
    {
        FoundDirectoryEntry =
            GetDirectoryEntryOfDirectoryInSector(&Disk->RootDirectory.Sectors[SectorIndex], DirectoryName);

        if (FoundDirectoryEntry)
        {
            break;
        }
    }

    return FoundDirectoryEntry;
}

directory_entry far *
GetDirectoryEntryOfFileByPath
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    char far *FullFilePath
)
{
    if (StringLengthFar(FullFilePath) == 1)
    {
        return NULL;
    }

    file_path_node far *FirstNode = CreateFilePathSegmentList(FullFilePath, MemoryArena);
    if (!FirstNode)
    {
        return NULL;
    }

    file_path_node far *CurrentNode = FirstNode;

    char LocalFileName[8];
    char LocalFileExtension[3];
    MemoryZeroNear(LocalFileName, 8);
    MemoryZeroNear(LocalFileExtension, 3);

    GetFileNameAndExtensionFromString
    (
        CurrentNode->FileName,
        (char far *)LocalFileName, 8,
        (char far *)LocalFileExtension, 3
    );

    directory_entry far *CurrentEntry = GetDirectoryEntryOfFileInRootDirectory
    (
        Disk,
        (char far *)LocalFileName,
        (char far *)LocalFileExtension
    );

    if (CurrentEntry && !CurrentNode->ChildNode)
    {
        return CurrentEntry;
    }

    CurrentNode = CurrentNode->ChildNode;

    while (CurrentNode)
    {
        MemoryZeroNear(LocalFileName, ArrayCount(LocalFileName));
        MemoryZeroNear(LocalFileExtension, ArrayCount(LocalFileExtension));

        GetFileNameAndExtensionFromString
        (
            CurrentNode->FileName,
            (char far *)LocalFileName, 8,
            (char far *)LocalFileExtension, 3
        );

        CurrentEntry = GetDirectoryEntryOfFileInDirectory
        (
            Disk,
            MemoryArena,
            DiskParameters,
            CurrentEntry,
            LocalFileName,
            LocalFileExtension
        );

        if (CurrentEntry && !CurrentNode->ChildNode)
        {
            return CurrentEntry;
        }

        CurrentNode = CurrentNode->ChildNode;
    }

    return NULL;
}

void Fat12ListDirectorySector(sector far *Sector)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < FAT12_DIRECTORY_ENTRIES_IN_SECTOR;
        DirectoryEntryIndex++
    )
    {
        directory_entry far *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];
        if (DirectoryEntry->FileName[0] == FAT12_FILENAME_EMPTY_SLOT)
        {
        }
        else if (DirectoryEntry->FileName[0] == FAT12_FILENAME_DELETED_SLOT)
        {
            PrintFormatted("    > deleted file.\r\n");
        }
        else
        {
            if (DirectoryEntry->FileAttributes == FAT12_FILE_ATTRIBUTE_NORMAL)
            {
                char FileNameString[9];
                char FileExtensionString[4];

                MemoryCopyFarToNear(FileNameString, DirectoryEntry->FileName, 8);
                MemoryCopyFarToNear(FileExtensionString, DirectoryEntry->FileExtension, 3);

                FileNameString[8] = 0;
                FileExtensionString[3] = 0;

                PrintFormatted("    FILE:   %s.%s\r\n", FileNameString, FileExtensionString);
            }
            else if (DirectoryEntry->FileAttributes == FAT12_FILE_ATTRIBUTE_DIRECTORY)
            {
                char FileNameString[9];

                MemoryCopyFarToNear(FileNameString, DirectoryEntry->FileName, 8);
                FileNameString[8] = 0;
                PrintFormatted("     DIR:   %s\r\n", FileNameString);
            }
        }
    }
}

void Fat12ListDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    char far *DirectoryPath
)
{
    PrintFormatted("\r\nlisting %ls:\r\n", DirectoryPath);

    if
    (
        (StringLengthFar(DirectoryPath) == 1) &&
        (MemoryCompareFarToFar(DirectoryPath, "\\", 1) == 0)
    )
    {
        for (u32 SectorIndex = 0; SectorIndex < FAT12_SECTORS_IN_ROOT_DIRECTORY; SectorIndex++)
        {
            Fat12ListDirectorySector(&Disk->RootDirectory.Sectors[SectorIndex]);
        }
    }
    else
    {
        directory_entry far *DirectoryEntry = GetDirectoryEntryOfFileByPath
        (
            Disk, MemoryArena, DiskParameters, DirectoryPath
        );

        u16 CurrentClusterNumber = DirectoryEntry->ClusterNumberLowWord;
        u16 CurrentFatEntry = GetFatEntryFromClusterNumber(Disk, CurrentClusterNumber);

        for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
        {
            sector far *Sector = PushStruct(MemoryArena, sector);
            GetDiskSectorFromFatClusterNumber
            (
                DiskParameters,
                Sector,
                CurrentClusterNumber
            );
            Fat12ListDirectorySector(Sector);

            if (IsFatEntryEndOfFile(CurrentFatEntry))
            {
                break;
            }
            else
            {
                CurrentClusterNumber = CurrentFatEntry;
                CurrentFatEntry = GetFatEntryFromClusterNumber(Disk, CurrentClusterNumber);
            }
        }
    }
}