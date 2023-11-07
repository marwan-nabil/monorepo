void SetFatEntry(fat12_ram_disk far *Disk, u32 ClusterNumber, u16 FatEntry)
{
    u32 StartingByteIndex = ClusterNumber * 3 / 2;

    if ((ClusterNumber % 2) == 0)
    {
        Disk->Fat1.Bytes[StartingByteIndex] = (u8)FatEntry;
        Disk->Fat1.Bytes[StartingByteIndex + 1] &= (u8)0xF0;
        Disk->Fat1.Bytes[StartingByteIndex + 1] |= (u8)((FatEntry >> 8) & 0x000F);
    }
    else
    {
        Disk->Fat1.Bytes[StartingByteIndex] &= (u8)0x0F;
        Disk->Fat1.Bytes[StartingByteIndex] |= (u8)((FatEntry & 0x000F) << 4);
        Disk->Fat1.Bytes[StartingByteIndex + 1] = (u8)((FatEntry >> 4) & 0x00FF);
    }
}

u16 AllocateDiskClusters
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    void far *Memory,
    u32 Size
)
{
    u16 FirstAllocatedClusterNumber = 0;

    u32 ClustersNeeded = (Size + FAT12_SECTOR_SIZE - 1) / FAT12_SECTOR_SIZE;
    if (ClustersNeeded > CalculateNumberOfFreeClusters(Disk))
    {
        return 0;
    }

    u32 SizeLeft = Size;
    char far *ReadPointer = (char far *)Memory;
    u16 PreviousClusterNumber = 0;

    for (u32 ClusterIndex = 0; ClusterIndex < ClustersNeeded; ClusterIndex++)
    {
        u16 ClusterNumber = GetFirstFreeClusterNumber(Disk);

        SetFatEntry(Disk, ClusterNumber, FAT12_FAT_ENTRY_RESERVED_CLUSTER);

        if (PreviousClusterNumber)
        {
            SetFatEntry(Disk, PreviousClusterNumber, ClusterNumber);
        }
        else
        {
            FirstAllocatedClusterNumber = ClusterNumber;
        }

        u16 SectorIndex = TranslateClusterNumberToSectorIndex(ClusterNumber);

        u32 BytesToCopy = FAT12_SECTOR_SIZE;
        u32 BytesToZero = 0;

        if (SizeLeft < BytesToCopy)
        {
            BytesToCopy = SizeLeft;
            BytesToZero = FAT12_SECTOR_SIZE - SizeLeft;
        }

        sector far *RamSector = PushStruct(MemoryArena, sector);
        ReadDiskSectors(DiskParameters, SectorIndex, 1, RamSector->Bytes);

        if (ReadPointer)
        {
            MemoryCopyFarToFar(RamSector->Bytes, ReadPointer, BytesToCopy);
            ReadPointer += BytesToCopy;
            MemoryZeroFar(RamSector->Bytes + BytesToCopy, BytesToZero);
        }
        else
        {
            MemoryZeroFar(RamSector->Bytes, BytesToCopy + BytesToZero);
        }

        WriteDiskSectors(DiskParameters, SectorIndex, 1, RamSector->Bytes);

        SizeLeft -= BytesToCopy;

        if (SizeLeft == 0)
        {
            SetFatEntry(Disk, ClusterNumber, FAT12_FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END);
        }

        PreviousClusterNumber = ClusterNumber;
    }

    return FirstAllocatedClusterNumber;
}

b8 AllocateFileToDirectoryEntry
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    directory_entry far *DirectoryEntry,
    char far *FileName,
    char far *Extension,
    void far *Memory,
    u32 Size
)
{
    u16 ClusterNumber = AllocateDiskClusters(Disk, MemoryArena, DiskParameters, Memory, Size);
    if (ClusterNumber)
    {
        MemoryZeroFar(DirectoryEntry, sizeof(directory_entry));

        MemoryCopyFarToFar(DirectoryEntry->FileName, FileName, 8);
        MemoryCopyFarToFar(DirectoryEntry->FileExtension, Extension, 3);

        DirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_NORMAL;
        DirectoryEntry->FileSize = Size;
        DirectoryEntry->ClusterNumberLowWord = ClusterNumber;
        return TRUE;
    }

    return FALSE;
}

b8 AllocateDirectoryToDirectoryEntry
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    directory_entry far *DirectoryEntry,
    char far *DirectoryName
)
{
    u16 ClusterNumber = AllocateDiskClusters(Disk, MemoryArena, DiskParameters, 0, FAT12_SECTOR_SIZE);
    if (ClusterNumber)
    {
        MemoryZeroFar(DirectoryEntry, sizeof(directory_entry));

        MemoryCopyFarToFar(DirectoryEntry->FileName, DirectoryName, 8);
        DirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_DIRECTORY;
        DirectoryEntry->FileSize = 0;
        DirectoryEntry->ClusterNumberLowWord = ClusterNumber;
        return TRUE;
    }

    return FALSE;
}

directory_entry far *
AddFileToRootDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    char far *FileName,
    char far *Extension,
    void far *Memory,
    u32 Size
)
{
    directory_entry far *FoundDirectoryEntry = GetFirstFreeDirectoryEntryInRootDirectory(Disk);
    if (!FoundDirectoryEntry)
    {
        return NULL;
    }

    b8 Result = AllocateFileToDirectoryEntry
    (
        Disk,
        MemoryArena,
        DiskParameters,
        FoundDirectoryEntry,
        FileName,
        Extension,
        Memory,
        Size
    );

    if (Result)
    {
        return FoundDirectoryEntry;
    }
    else
    {
        return NULL;
    }
}

directory_entry far *
AddDirectoryToRootDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    char far *DirectoryName
)
{
    directory_entry far *FoundDirectoryEntry = GetFirstFreeDirectoryEntryInRootDirectory(Disk);
    if (!FoundDirectoryEntry)
    {
        return NULL;
    }

    b8 Result = AllocateDirectoryToDirectoryEntry
    (
        Disk,
        MemoryArena,
        DiskParameters,
        FoundDirectoryEntry,
        DirectoryName
    );

    if (Result)
    {
        return FoundDirectoryEntry;
    }
    else
    {
        return NULL;
    }
}

directory_entry far *
AddFileToDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    directory_entry far *Directory,
    char far *FileName,
    char far *Extension,
    void far *Memory,
    u32 Size
)
{
    directory_entry far *FoundDirectoryEntry =
        GetFirstFreeDirectoryEntryInDirectory(Disk, MemoryArena, DiskParameters, Directory);
    if (!FoundDirectoryEntry)
    {
        return NULL;
    }

    b8 Result = AllocateFileToDirectoryEntry
    (
        Disk,
        MemoryArena,
        DiskParameters,
        FoundDirectoryEntry,
        FileName,
        Extension,
        Memory,
        Size
    );

    if (Result)
    {
        return FoundDirectoryEntry;
    }
    else
    {
        return NULL;
    }
}

directory_entry far *
AddDirectoryToDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    directory_entry far *Directory,
    char far *DirectoryName
)
{
    directory_entry far *FoundDirectoryEntry =
        GetFirstFreeDirectoryEntryInDirectory(Disk, MemoryArena, DiskParameters, Directory);
    if (!FoundDirectoryEntry)
    {
        return NULL;
    }

    b8 Result = AllocateDirectoryToDirectoryEntry
    (
        Disk,
        MemoryArena,
        DiskParameters,
        FoundDirectoryEntry,
        DirectoryName
    );
    if (Result)
    {
        return FoundDirectoryEntry;
    }
    else
    {
        return NULL;
    }
}