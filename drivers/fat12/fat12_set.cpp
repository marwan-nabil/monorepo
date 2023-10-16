inline void
SetFatEntry(fat12_disk *Disk, u32 LogicalCluster, u16 FatEntry)
{
    Assert(LogicalCluster >= 2);
    u32 StartingByteIndex = LogicalCluster * 3 / 2;

    if ((LogicalCluster % 2) == 0)
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

inline u16 AllocateSectorsFromMemory(fat12_disk *Disk, void *Memory, u32 Size)
{
    u16 FirstLogicalCluster = 0;

    u32 ClustersNeeded = (Size + FAT12_DISK_CLUSTER_SIZE - 1) / FAT12_DISK_CLUSTER_SIZE;
    if (ClustersNeeded > GetNumberOfFreeLogicalClusters(Disk))
    {
        return 0;
    }

    u32 SizeLeft = Size;
    char *ReadPointer = (char *)Memory;

    u16 PreviousLogicalCluster = 0;

    while (ClustersNeeded)
    {
        u16 LogicalCluster = GetFirstFreeLogicalCluster(Disk);
        Assert(LogicalCluster);

        SetFatEntry(Disk, LogicalCluster, FAT12_FAT_ENTRY_RESERVED_CLUSTER);

        if (PreviousLogicalCluster)
        {
            SetFatEntry(Disk, PreviousLogicalCluster, LogicalCluster);
        }
        else
        {
            FirstLogicalCluster = LogicalCluster;
        }

        u16 PhysicalCluster = TranslateLogicalCluster(LogicalCluster);

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
            SetFatEntry(Disk, LogicalCluster, FAT12_FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END);
        }
    }

    return FirstLogicalCluster;
}

u16 AddFileToDirectory
(
    fat12_disk *Disk, u16 DirectoryLogicalCluster,
    void *Memory, u32 Size, char *FileName, char *Extension
)
{
    directory_entry *FreeDirectoryEntry = GetFirstFreeEntryInDirectory(Disk, DirectoryLogicalCluster);
    if (!FreeDirectoryEntry)
    {
        return 0;
    }

    u16 FirstLogicalCluster = AllocateSectorsFromMemory(Disk, Memory, Size);
    if (FirstLogicalCluster)
    {
        memcpy(FreeDirectoryEntry->FileName, FileName, 8);
        memcpy(FreeDirectoryEntry->FileExtension, Extension, 3);
        FreeDirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_NORMAL;
        FreeDirectoryEntry->FileSize = Size;
        FreeDirectoryEntry->FirstLogicalCluster = FirstLogicalCluster;
    }

    return FirstLogicalCluster;
}

u16 AddDirectoryToDirectory
(
    fat12_disk *Disk, u16 DirectoryLogicalCluster, char *DirectoryName
)
{
    directory_entry *FreeDirectoryEntry = GetFirstFreeEntryInDirectory(Disk, DirectoryLogicalCluster);
    if (!FreeDirectoryEntry)
    {
        return 0;
    }

    u16 FirstLogicalCluster = AllocateSectorsFromMemory(Disk, 0, 512);
    if (FirstLogicalCluster)
    {
        StringCchCat((char *)FreeDirectoryEntry->FileName, 8, DirectoryName);
        *FreeDirectoryEntry->FileExtension = {};

        FreeDirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_DIRECTORY;
        FreeDirectoryEntry->FileSize = 0;
        FreeDirectoryEntry->FirstLogicalCluster = FirstLogicalCluster;
    }

    return FirstLogicalCluster;
}

u16 AddDirectoryToRootDirectory
(
    fat12_disk *Disk, char *DirectoryName
)
{
    directory_entry *FreeDirectoryEntry = GetFirstFreeEntryInRootDirectory(Disk);
    if (!FreeDirectoryEntry)
    {
        return 0;
    }

    u16 FirstLogicalCluster = AllocateSectorsFromMemory(Disk, 0, 512);
    if (FirstLogicalCluster)
    {
        StringCchCat((char *)FreeDirectoryEntry->FileName, 8, DirectoryName);
        *FreeDirectoryEntry->FileExtension = {};

        FreeDirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_DIRECTORY;
        FreeDirectoryEntry->FileSize = 0;
        FreeDirectoryEntry->FirstLogicalCluster = FirstLogicalCluster;
    }

    return FirstLogicalCluster;
}