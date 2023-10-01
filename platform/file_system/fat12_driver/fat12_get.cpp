inline u16 TranslateLogicalCluster(u16 LogicalCluster)
{
    Assert(LogicalCluster >= 2);
    u16 PhysicalCluster = 33 + LogicalCluster - 2;
    return PhysicalCluster;
}

inline b32 IsFatEntryEndOfFile(u16 FatEntry)
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

inline sector *
GetSector(fat12_disk *Disk, u16 LogicalCluster)
{
    Assert(LogicalCluster >= 2);
    sector *Result = &Disk->Sectors[TranslateLogicalCluster(LogicalCluster)];
    return Result;
}

inline u16
GetFatEntry(fat12_disk *Disk, u32 LogicalCluster)
{
    Assert(LogicalCluster >= 2);
    u16 Result = 0;
    u32 StartingByteIndex = LogicalCluster * 3 / 2;

    if ((LogicalCluster % 2) == 0)
    {
        Result = (u16)Disk->Fat1.Bytes[StartingByteIndex];
        Result |= ((u16)Disk->Fat1.Bytes[StartingByteIndex + 1] & 0x000F) << 8;
    }
    else
    {
        Result = (u16)(Disk->Fat1.Bytes[StartingByteIndex] & 0xF0) >> 4;
        Result |= ((u16)Disk->Fat1.Bytes[StartingByteIndex + 1]) << 4;
    }

    return Result;
}

inline u16
GetFirstFreeLogicalCluster(fat12_disk *Disk)
{
    for (u16 LogicalCluster = 2; LogicalCluster < FAT12_ENTRIES_PER_FAT; LogicalCluster++)
    {
        u16 FatEntry = GetFatEntry(Disk, LogicalCluster);
        if (FatEntry == FAT12_FAT_ENTRY_FREE_CLUSTER)
        {
            return LogicalCluster;
        }
    }
    return 0;
}

inline u32
GetNumberOfFreeLogicalClusters(fat12_disk *Disk)
{
    u32 Result = 0;

    for (u16 LogicalCluster = 2; LogicalCluster < FAT12_ENTRIES_PER_FAT; LogicalCluster++)
    {
        u16 FatEntry = GetFatEntry(Disk, LogicalCluster);
        if (FatEntry == FAT12_FAT_ENTRY_FREE_CLUSTER)
        {
            Result++;
        }
    }

    return Result;
}

inline directory_entry *
GetFirstFreeDirectoryEntryInSector(sector *Sector)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < ArrayCount(Sector->DirectoryEntries);
        DirectoryEntryIndex++
    )
    {
        directory_entry *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];
        if (DirectoryEntry->FileName[0] == FAT12_FILENAME_EMPTY_SLOT)
        {
            return DirectoryEntry;
        }
    }

    return NULL;
}

inline directory_entry *
GetFirstFreeEntryInDirectory(fat12_disk *Disk, u16 DirectoryLogicalCluster)
{
    directory_entry *FirstFreeDirectoryEntry = NULL;

    u16 CurrentLogicalCluster = DirectoryLogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(Disk, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetSector(Disk, CurrentLogicalCluster);
        FirstFreeDirectoryEntry = GetFirstFreeDirectoryEntryInSector(PhysicalSector);

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
                CurrentLogicalCluster = CurrentFatEntry;
                CurrentFatEntry = GetFatEntry(Disk, CurrentLogicalCluster);
            }
        }
    }

    return FirstFreeDirectoryEntry;
}

inline directory_entry *
GetFirstFreeEntryInRootDirectory(fat12_disk *Disk)
{
    directory_entry *FirstFreeDirectoryEntry = NULL;

    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
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

inline directory_entry *
GetDirectoryEntryOfFileInSector(sector *Sector, u16 FileLogicalCluster)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < ArrayCount(Sector->DirectoryEntries);
        DirectoryEntryIndex++
    )
    {
        directory_entry *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];
        if (DirectoryEntry->FirstLogicalCluster == FileLogicalCluster)
        {
            return DirectoryEntry;
        }
    }

    return NULL;
}

inline directory_entry *
GetDirectoryEntryOfFileInSector(sector *Sector, char *FileName, char *Extension)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < ArrayCount(Sector->DirectoryEntries);
        DirectoryEntryIndex++
    )
    {
        directory_entry *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];
        if
        (
            (memcmp(DirectoryEntry->FileName, FileName, 8) == 0) &&
            (memcmp(DirectoryEntry->FileExtension, Extension, 3) == 0)
        )
        {
            return DirectoryEntry;
        }
    }

    return NULL;
}

inline directory_entry *
GetDirectoryEntryOfFileInDirectory(fat12_disk *Disk, u16 DirectoryLogicalCluster, u16 FileLogicalCluster)
{
    directory_entry *FoundDirectoryEntry = NULL;

    u16 CurrentLogicalCluster = DirectoryLogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(Disk, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetSector(Disk, CurrentLogicalCluster);
        FoundDirectoryEntry = GetDirectoryEntryOfFileInSector(PhysicalSector, FileLogicalCluster);

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
                CurrentLogicalCluster = CurrentFatEntry;
                CurrentFatEntry = GetFatEntry(Disk, CurrentLogicalCluster);
            }
        }
    }

    return FoundDirectoryEntry;
}

inline directory_entry *
GetDirectoryEntryOfFileInDirectory
(
    fat12_disk *Disk, u16 DirectoryLogicalCluster,
    char *FileName, char *Extension
)
{
    directory_entry *FoundDirectoryEntry = NULL;

    u16 CurrentLogicalCluster = DirectoryLogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(Disk, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetSector(Disk, CurrentLogicalCluster);
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
                CurrentLogicalCluster = CurrentFatEntry;
                CurrentFatEntry = GetFatEntry(Disk, CurrentLogicalCluster);
            }
        }
    }

    return FoundDirectoryEntry;
}

inline directory_entry *
GetDirectoryEntryOfFileInRootDirectory(fat12_disk *Disk, u16 FileLogicalCluster)
{
    directory_entry *FoundDirectoryEntry = NULL;

    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
    {
        FoundDirectoryEntry =
            GetDirectoryEntryOfFileInSector(&Disk->RootDirectory.Sectors[SectorIndex], FileLogicalCluster);

        if (FoundDirectoryEntry)
        {
            break;
        }
    }

    return FoundDirectoryEntry;
}

inline directory_entry *
GetDirectoryEntryOfFileInRootDirectory(fat12_disk *Disk, char *FileName, char *Extension)
{
    directory_entry *FoundDirectoryEntry = NULL;

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