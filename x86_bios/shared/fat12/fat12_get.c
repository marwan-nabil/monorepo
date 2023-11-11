u16 TranslateClusterNumberToSectorIndex(u16 ClusterNumber)
{
    u16 PhysicalSectorIndex = FAT12_DATA_AREA_START_SECTOR + ClusterNumber - 2;
    return PhysicalSectorIndex;
}

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

sector far *GetSectorFromClusterNumber
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    u16 ClusterNumber
)
{
    u16 LBA = TranslateClusterNumberToSectorIndex(ClusterNumber);
    sector far *Result = PushStruct(MemoryArena, sector);
    ReadDiskSectors(DiskParameters, LBA, 1, (u8 far *)Result);
    return Result;
}

u16 GetFatEntry(fat12_ram_disk far *Disk, u32 ClusterNumber)
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
        u16 FatEntry = GetFatEntry(Disk, ClusterNumber);
        if (FatEntry == FAT12_FAT_ENTRY_FREE_CLUSTER)
        {
            return ClusterNumber;
        }
    }
    return 0;
}

u32 CalculateNumberOfFreeClusters(fat12_ram_disk far *Disk)
{
    u32 Result = 0;

    for (u16 ClusterNumber = 2; ClusterNumber < FAT12_FAT_ENTRIES_IN_FAT; ClusterNumber++)
    {
        u16 FatEntry = GetFatEntry(Disk, ClusterNumber);
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
GetFirstFreeDirectoryEntryInDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    directory_entry far *Directory
)
{
    directory_entry far *FirstFreeDirectoryEntry = NULL;

    u16 CurrentClusterNumber = Directory->ClusterNumberLowWord;
    u16 CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);

    for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
    {
        sector far *Sector =
            GetSectorFromClusterNumber(Disk, MemoryArena, DiskParameters, CurrentClusterNumber);
        FirstFreeDirectoryEntry = GetFirstFreeDirectoryEntryInSector(Sector);

        if (FirstFreeDirectoryEntry)
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
                CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);
            }
        }
    }

    return FirstFreeDirectoryEntry;
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
    // NOTE: this function is buggy, currently debugging it
    PrintFormatted("DirectoryName: %ls\r\n", DirectoryName);

    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < FAT12_DIRECTORY_ENTRIES_IN_SECTOR;
        DirectoryEntryIndex++
    )
    {
        directory_entry *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];

        if (MemoryCompareFarToFar(DirectoryEntry->FileName, DirectoryName, 8) == 0)
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
    u16 CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);

    for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
    {
        sector far *PhysicalSector = GetSectorFromClusterNumber
        (
            Disk,
            MemoryArena,
            DiskParameters,
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
                CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);
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
    u16 CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);

    for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
    {
        sector far *PhysicalSector =
            GetSectorFromClusterNumber(Disk, MemoryArena, DiskParameters, CurrentClusterNumber);
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
                CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);
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