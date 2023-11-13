void InitializeFat12RamDisk
(
    disk_parameters far *DiskParameters,
    memory_arena far *MemoryArena,
    fat12_ram_disk far *RamDisk
)
{
    boot_sector far *RamBootSector = PushStruct(MemoryArena, boot_sector);
    ReadDiskSectors(DiskParameters, 0, 1, RamBootSector);
    MemoryCopyFarToFar(&RamDisk->BootSectorHeader, RamBootSector, sizeof(boot_sector_header));
    FreeMemoryArena(MemoryArena);

    ReadDiskSectors(DiskParameters, 1, FAT12_SECTORS_IN_FAT, &RamDisk->Fat);
    ReadDiskSectors
    (
        DiskParameters,
        1 + (2 * FAT12_SECTORS_IN_FAT),
        FAT12_SECTORS_IN_ROOT_DIRECTORY,
        &RamDisk->RootDirectory
    );
}

void SetFatEntry(fat12_ram_disk far *Disk, u32 ClusterNumber, u16 FatEntry)
{
    u32 StartingByteIndex = ClusterNumber * 3 / 2;

    if ((ClusterNumber % 2) == 0)
    {
        Disk->Fat.Bytes[StartingByteIndex] = (u8)FatEntry;
        Disk->Fat.Bytes[StartingByteIndex + 1] &= (u8)0xF0;
        Disk->Fat.Bytes[StartingByteIndex + 1] |= (u8)((FatEntry >> 8) & 0x000F);
    }
    else
    {
        Disk->Fat.Bytes[StartingByteIndex] &= (u8)0x0F;
        Disk->Fat.Bytes[StartingByteIndex] |= (u8)((FatEntry & 0x000F) << 4);
        Disk->Fat.Bytes[StartingByteIndex + 1] = (u8)((FatEntry >> 4) & 0x00FF);
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
    if (ClustersNeeded > CalculateFreeClusterNumbers(Disk))
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

        u16 SectorIndex = TranslateFatClusterNumberToSectorIndex(ClusterNumber);

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

directory_entry far *
Fat12AddFileByPath
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    char far *FullFilePath,
    void far *Memory,
    u32 Size
)
{
    if (StringLengthFar(FullFilePath) == 1)
    {
        return NULL;
    }

    file_path_node far *CurrentPathNode = CreateFilePathSegmentList(FullFilePath, MemoryArena);

    char LocalFileName[8];
    char LocalFileExtension[3];
    MemoryZeroNear(LocalFileName, 8);
    MemoryZeroNear(LocalFileExtension, 3);

    GetFileNameAndExtensionFromString
    (
        CurrentPathNode->FileName, LocalFileName, 8, LocalFileExtension, 3
    );

    if (!CurrentPathNode->ChildNode)
    {
        directory_entry far *FileDirectoryEntry = GetDirectoryEntryOfFileInRootDirectory
        (
            Disk,
            LocalFileName,
            LocalFileExtension
        );

        if (FileDirectoryEntry)
        {
            return NULL;
        }
        else
        {
            FileDirectoryEntry = AddFileToRootDirectory
            (
                Disk,
                MemoryArena,
                DiskParameters,
                LocalFileName,
                LocalFileExtension,
                Memory,
                Size
            );
            return FileDirectoryEntry;
        }
    }

    directory_entry far *CurrentDirectoryEntry =
        GetDirectoryEntryOfDirectoryInRootDirectory(Disk, LocalFileName);
    CurrentPathNode = CurrentPathNode->ChildNode;

    while (CurrentPathNode)
    {
        MemoryZeroNear(LocalFileName, ArrayCount(LocalFileName));
        MemoryZeroNear(LocalFileExtension, ArrayCount(LocalFileExtension));

        GetFileNameAndExtensionFromString
        (
            CurrentPathNode->FileName, LocalFileName, 8, LocalFileExtension, 3
        );

        if (!CurrentPathNode->ChildNode)
        {
            directory_entry far *FileDirectoryEntry = GetDirectoryEntryOfFileInDirectory
            (
                Disk,
                MemoryArena,
                DiskParameters,
                CurrentDirectoryEntry,
                LocalFileName,
                LocalFileExtension
            );

            if (FileDirectoryEntry)
            {
                return NULL;
            }
            else
            {
                FileDirectoryEntry = AddFileToDirectory
                (
                    Disk,
                    MemoryArena,
                    DiskParameters,
                    CurrentDirectoryEntry,
                    LocalFileName,
                    LocalFileExtension,
                    Memory,
                    Size
                );
                return FileDirectoryEntry;
            }
        }
        else
        {
            CurrentDirectoryEntry = GetDirectoryEntryOfDirectoryInDirectory
            (
                Disk,
                MemoryArena,
                DiskParameters,
                CurrentDirectoryEntry,
                LocalFileName
            );
            CurrentPathNode = CurrentPathNode->ChildNode;
        }
    }

    return NULL;
}

directory_entry far *
Fat12AddDirectoryByPath
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters far *DiskParameters,
    char far *DirectoryPath
)
{
    if (StringLengthFar(DirectoryPath) == 1)
    {
        return NULL;
    }

    file_path_node far *CurrentPathNode = CreateFilePathSegmentList(DirectoryPath, MemoryArena);

    if (!CurrentPathNode->ChildNode)
    {
        char LocalDirectoryName[8];
        MemoryZeroNear(LocalDirectoryName, 8);

        FillFixedSizeStringBuffer
        (
            LocalDirectoryName,
            ArrayCount(LocalDirectoryName),
            CurrentPathNode->FileName
        );

        directory_entry far *DirectoryDirectoryEntry = GetDirectoryEntryOfDirectoryInRootDirectory
        (
            Disk,
            LocalDirectoryName
        );

        if (DirectoryDirectoryEntry)
        {
            return NULL;
        }
        else
        {

            directory_entry far *DirectoryDirectoryEntry = AddDirectoryToRootDirectory
            (
                Disk,
                MemoryArena,
                DiskParameters,
                LocalDirectoryName
            );
            return DirectoryDirectoryEntry;
        }
    }

    directory_entry far *CurrentDirectoryEntry =
        GetDirectoryEntryOfDirectoryInRootDirectory(Disk, CurrentPathNode->FileName);
    CurrentPathNode = CurrentPathNode->ChildNode;

    while (CurrentPathNode)
    {
        if (!CurrentPathNode->ChildNode)
        {
            char LocalDirectoryName[8];
            MemoryZeroNear(LocalDirectoryName, 8);

            FillFixedSizeStringBuffer
            (
                LocalDirectoryName,
                ArrayCount(LocalDirectoryName),
                CurrentPathNode->FileName
            );

            directory_entry far *DirectoryDirectoryEntry = GetDirectoryEntryOfDirectoryInDirectory
            (
                Disk,
                MemoryArena,
                DiskParameters,
                CurrentDirectoryEntry,
                LocalDirectoryName
            );

            if (DirectoryDirectoryEntry)
            {
                return NULL;
            }
            else
            {
                DirectoryDirectoryEntry = AddDirectoryToDirectory
                (
                    Disk,
                    MemoryArena,
                    DiskParameters,
                    CurrentDirectoryEntry,
                    LocalDirectoryName
                );
                return DirectoryDirectoryEntry;
            }
        }
        else
        {
            CurrentDirectoryEntry = GetDirectoryEntryOfDirectoryInDirectory
            (
                Disk,
                MemoryArena,
                DiskParameters,
                CurrentDirectoryEntry,
                CurrentPathNode->FileName
            );
            CurrentPathNode = CurrentPathNode->ChildNode;
        }
    }

    return NULL;
}