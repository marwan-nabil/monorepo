inline u16 TranslateCluster(u16 LogicalCluster)
{
    Assert(LogicalCluster >= 2);
    u16 PhysicalCluster = 33 + LogicalCluster - 2;
    return PhysicalCluster;
}

inline sector *GetPhysicalSector(fat12_disk *Disk, u16 LogicalCluster)
{
    sector *Result = &Disk->Sectors[TranslateCluster(LogicalCluster)];
    return Result;
}

inline u16 GetFatEntry(file_allocation_table *Fat, u32 LogicalCluster)
{
    u16 Result = 0;
    u32 StartingByteIndex = LogicalCluster * 3 / 2;

    if ((LogicalCluster % 2) == 0)
    {
        Result = (u16)Fat->Bytes[StartingByteIndex];
        Result |= ((u16)Fat->Bytes[StartingByteIndex + 1] & 0x000F) << 8;
    }
    else
    {
        Result = (u16)(Fat->Bytes[StartingByteIndex] & 0xF0) >> 4;
        Result |= ((u16)Fat->Bytes[StartingByteIndex + 1]) << 4;
    }

    return Result;
}

inline void SetFatEntry(file_allocation_table *Fat, u32 LogicalCluster, u16 Value)
{
    u32 StartingByteIndex = LogicalCluster * 3 / 2;

    if ((LogicalCluster % 2) == 0)
    {
        Fat->Bytes[StartingByteIndex] = (u8)(Value);
        Fat->Bytes[StartingByteIndex + 1] &= (u8)0xF0;
        Fat->Bytes[StartingByteIndex + 1] |= (u8)((Value >> 8) & 0x000F);
    }
    else
    {
        Fat->Bytes[StartingByteIndex] &= (u8)0x0F;
        Fat->Bytes[StartingByteIndex] |= (u8)((Value & 0x000F) << 4);
        Fat->Bytes[StartingByteIndex + 1] = (u8)((Value >> 4) & 0x00FF);
    }
}

inline b32 IsFatEntryEndOfFile(u16 FatEntry)
{
    if
    (
        (FatEntry >= FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_START) &&
        (FatEntry <= FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END)
    )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

inline u16
GetFirstEmptyLogicalClusterInFat(file_allocation_table *Fat)
{
    for (u16 LogicalCluster = 2; LogicalCluster < FAT12_ENTRIES_PER_FAT; LogicalCluster++)
    {
        u16 FatEntry = GetFatEntry(Fat, LogicalCluster);
        if (FatEntry == FAT_ENTRY_FREE_CLUSTER)
        {
            return LogicalCluster;
        }
    }

    printf("ERROR: could not find a single empty logical cluster in the file allocation table.\n");
    return 0;
}

inline directory_entry *GetFirstFreeDirectoryEntryInSector(sector *Sector)
{
    for
    (
        u32 DirectoryEntryIndex = 0;
        DirectoryEntryIndex < ArrayCount(Sector->DirectoryEntries);
        DirectoryEntryIndex++
    )
    {
        directory_entry *DirectoryEntry = &Sector->DirectoryEntries[DirectoryEntryIndex];
        if (DirectoryEntry->FileName[0] == FILENAME_EMPTY_SLOT)
        {
            return DirectoryEntry;
        }
    }

    return NULL;
}

b32 AllocateSectors(fat12_disk *Disk, void *Memory, u32 Size, u16 *AllocatedAreaFirstCluster)
{
    u16 FirstLogicalCluster = 0;

    u32 ClustersNeeded = (Size + FAT12_DISK_CLUSTER_SIZE - 1) / FAT12_DISK_CLUSTER_SIZE;
    u32 SizeLeft = Size;
    char *ReadPointer = (char *)Memory;

    u16 PreviousLogicalCluster = 0;

    while (ClustersNeeded)
    {
        u16 LogicalCluster = GetFirstEmptyLogicalClusterInFat(&Disk->Fat1);
        SetFatEntry(&Disk->Fat1, LogicalCluster, FAT_ENTRY_RESERVED_CLUSTER);

        if (PreviousLogicalCluster)
        {
            SetFatEntry(&Disk->Fat1, PreviousLogicalCluster, LogicalCluster);
        }
        else
        {
            FirstLogicalCluster = LogicalCluster;
        }

        u16 PhysicalCluster = TranslateCluster(LogicalCluster);

        u32 BytesToCopy = 512;
        u32 BytesToZero = 0;

        if (SizeLeft < 512)
        {
            BytesToCopy = SizeLeft;
            BytesToZero = 512 - SizeLeft;
        }

        if (ReadPointer)
        {
            memcpy(Disk->Sectors[PhysicalCluster].Bytes, ReadPointer, BytesToCopy);
            ReadPointer += BytesToCopy;
            ZeroMemory(Disk->Sectors[PhysicalCluster].Bytes + BytesToCopy, BytesToZero);
        }
        else
        {
            ZeroMemory(Disk->Sectors[PhysicalCluster].Bytes, BytesToCopy + BytesToZero);
        }

        SizeLeft -= BytesToCopy;
        ClustersNeeded--;

        PreviousLogicalCluster = LogicalCluster;

        if (SizeLeft == 0)
        {
            SetFatEntry(&Disk->Fat1, LogicalCluster, FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END);
        }
    }

    if (FirstLogicalCluster)
    {
        *AllocatedAreaFirstCluster = FirstLogicalCluster;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

b32 AddFileToDirectorySector(fat12_disk *Disk, sector *DirectorySector, ram_file File)
{
    directory_entry *DirectoryEntry =
        GetFirstFreeDirectoryEntryInSector(DirectorySector);

    if (!DirectoryEntry)
    {
        return FALSE;
    }

    b32 AllocationSuccess = AllocateSectors(Disk, File.Memory, File.Size, &DirectoryEntry->FirstLogicalCluster);
    if (AllocationSuccess)
    {
        memcpy(DirectoryEntry->FileName, File.Name, 8);
        memcpy(DirectoryEntry->FileExtension, File.Extension, 3);
        DirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_NORMAL;
        DirectoryEntry->FileSize = File.Size;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

b32 AddDirectoryToDirectorySector(fat12_disk *Disk, sector *DirectorySector, const char *DirectoryName)
{
    directory_entry *DirectoryEntry =
        GetFirstFreeDirectoryEntryInSector(DirectorySector);

    if (!DirectoryEntry)
    {
        return FALSE;
    }

    b32 AllocationSuccess = AllocateSectors(Disk, 0, 512, &DirectoryEntry->FirstLogicalCluster);
    if (AllocationSuccess)
    {
        memcpy(DirectoryEntry->FileName, DirectoryName, 8);
        memset(DirectoryEntry->FileExtension, 0, 3);
        DirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_DIRECTORY;
        DirectoryEntry->FileSize = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

b32 AddFileToDirectory(fat12_disk *Disk, u16 LogicalCluster, ram_file File)
{
    u16 CurrentLogicalCluster = LogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetPhysicalSector(Disk, CurrentLogicalCluster);
        b32 FileAdded = AddFileToDirectorySector(Disk, PhysicalSector, File);

        if (FileAdded)
        {
            break;
        }
        else
        {
            if (IsFatEntryEndOfFile(CurrentFatEntry))
            {
                return FALSE;
            }
            else
            {
                CurrentLogicalCluster = CurrentFatEntry;
                CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);
            }
        }
    }

    return TRUE;
}

b32 AddDirectoryToDirectory(fat12_disk *Disk, u16 LogicalCluster, const char *DirectoryName)
{
    u16 CurrentLogicalCluster = LogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetPhysicalSector(Disk, CurrentLogicalCluster);
        b32 FileAdded = AddDirectoryToDirectorySector(Disk, PhysicalSector, DirectoryName);

        if (FileAdded)
        {
            break;
        }
        else
        {
            if (IsFatEntryEndOfFile(CurrentFatEntry))
            {
                return FALSE;
            }
            else
            {
                CurrentLogicalCluster = CurrentFatEntry;
                CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);
            }
        }
    }

    return TRUE;
}

b32 AddFileToRootDirectory(fat12_disk *Disk, ram_file File)
{
    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
    {
        b32 FileAdded = AddFileToDirectorySector(Disk, &Disk->RootDirectory.Sectors[SectorIndex], File);

        if (FileAdded)
        {
            return TRUE;
        }
    }

    return FALSE;
}

b32 AddDirectoryToRootDirectory(fat12_disk *Disk, const char *DirectoryName)
{
    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
    {
        b32 FileAdded = AddDirectoryToDirectorySector(Disk, &Disk->RootDirectory.Sectors[SectorIndex], DirectoryName);

        if (FileAdded)
        {
            return TRUE;
        }
    }

    return FALSE;
}