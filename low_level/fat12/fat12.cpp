// TODO: improve error handling

inline u16 TranslateCluster(u16 LogicalCluster)
{
    Assert(LogicalCluster >= 2);
    u16 PhysicalCluster = 33 + LogicalCluster - 2;
    return PhysicalCluster;
}

inline sector *GetPhysicalCluster(fat12_disk *Disk, u16 LogicalCluster)
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

inline u16
GetFirstEmptyLogicalClusterInFat(file_allocation_table *Fat)
{
    for (u16 LogicalCluster = 2; LogicalCluster < FAT12_ENTRIES_PER_FAT; LogicalCluster++)
    {
        u16 FatEntry = GetFatEntry(Fat, LogicalCluster);
        if (FatEntry == FAT12_FAT_ENTRY_FREE_CLUSTER)
        {
            return LogicalCluster;
        }
    }
    return 0;
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

inline void
FreeFilePathSegmentList(file_path_node *RootNode)
{
    file_path_node *CurrentNode = RootNode;
    file_path_node *ChildNode;

    while (CurrentNode)
    {
        ChildNode = CurrentNode->ChildNode;
        free(CurrentNode);
        CurrentNode = ChildNode;
    }
}

inline file_path_node *
CreateFilePathSegmentList(char *FileFullPath)
{
    char LocalPathBuffer[FAT12_MAX_PATH];
    ZeroMemory(LocalPathBuffer, ArrayCount(LocalPathBuffer));
    StringCchCat(LocalPathBuffer, ArrayCount(LocalPathBuffer), FileFullPath);

    u32 PathLength = StringLength(LocalPathBuffer);

    file_path_node *CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
    *CurrentFilePathNode = {};
    file_path_node *LastFilePathNode = 0;

    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (LocalPathBuffer[CharIndex] == '\\')
        {
            char PathSegment[FAT12_MAX_PATH];
            ZeroMemory(PathSegment, ArrayCount(PathSegment));
            StringCchCat(PathSegment, FAT12_MAX_PATH, &LocalPathBuffer[CharIndex + 1]);
            ZeroMemory(&LocalPathBuffer[CharIndex], StringLength(&LocalPathBuffer[CharIndex]));

            if (!CurrentFilePathNode)
            {
                CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
                *CurrentFilePathNode = {};
            }

            StringCchCat(CurrentFilePathNode->NodeName, FAT12_MAX_PATH, PathSegment);
            CurrentFilePathNode->ChildNode = LastFilePathNode;

            LastFilePathNode = CurrentFilePathNode;
            CurrentFilePathNode = 0;
        }
    }

    return LastFilePathNode;
}

// TODO: implement cluster getters by file path
// u16 GetFirstLogicalClusterOfFile(fat12_disk *Disk, char *FullFilePath)
// {
//     file_path_node *CurrentNode = CreateFilePathSegmentsList(FullFilePath);

//     while (CurrentNode)
//     {
//         GetDirectorySector(CurrentNode->NodeName);
//         CurrentNode = CurrentNode->ChildNode;
//     }
// }

u16 AllocateSectors(fat12_disk *Disk, void *Memory, u32 Size)
{
    u16 FirstLogicalCluster = 0;

    u32 ClustersNeeded = (Size + FAT12_DISK_CLUSTER_SIZE - 1) / FAT12_DISK_CLUSTER_SIZE;
    u32 SizeLeft = Size;
    char *ReadPointer = (char *)Memory;

    u16 PreviousLogicalCluster = 0;

    while (ClustersNeeded)
    {
        u16 LogicalCluster = GetFirstEmptyLogicalClusterInFat(&Disk->Fat1);
        SetFatEntry(&Disk->Fat1, LogicalCluster, FAT12_FAT_ENTRY_RESERVED_CLUSTER);

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
            SetFatEntry(&Disk->Fat1, LogicalCluster, FAT12_FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END);
        }
    }

    return FirstLogicalCluster;
}

u16 AddFileToDirectorySector
(
    fat12_disk *Disk, sector *DirectorySector, ram_file File
)
{
    directory_entry *DirectoryEntry =
        GetFirstFreeDirectoryEntryInSector(DirectorySector);

    if (!DirectoryEntry)
    {
        return 0;
    }

    u16 FirstLogicalCluster = AllocateSectors(Disk, File.Memory, File.Size);
    if (FirstLogicalCluster)
    {
        memcpy(DirectoryEntry->FileName, File.Name, 8);
        memcpy(DirectoryEntry->FileExtension, File.Extension, 3);
        DirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_NORMAL;
        DirectoryEntry->FileSize = File.Size;
        DirectoryEntry->FirstLogicalCluster = FirstLogicalCluster;
    }

    return FirstLogicalCluster;
}

u16 AddDirectoryToDirectorySector
(
    fat12_disk *Disk, sector *DirectorySector, const char *DirectoryName
)
{
    directory_entry *DirectoryEntry =
        GetFirstFreeDirectoryEntryInSector(DirectorySector);

    if (!DirectoryEntry)
    {
        return FALSE;
    }

    u16 FirstLogicalCluster = AllocateSectors(Disk, 0, 512);
    if (FirstLogicalCluster)
    {
        StringCchCat((char *)DirectoryEntry->FileName, 8, DirectoryName);
        memset(DirectoryEntry->FileExtension, 0, 3);

        DirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_DIRECTORY;
        DirectoryEntry->FileSize = 0;
        DirectoryEntry->FirstLogicalCluster = FirstLogicalCluster;
    }

    return FirstLogicalCluster;
}

u16 AddFileToDirectory
(
    fat12_disk *Disk, u16 DirectoryLogicalCluster, ram_file File
)
{
    u16 FirstLogicalClusterForNewFile = 0;

    u16 CurrentLogicalCluster = DirectoryLogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetPhysicalCluster(Disk, CurrentLogicalCluster);
        FirstLogicalClusterForNewFile = AddFileToDirectorySector(Disk, PhysicalSector, File);

        if (FirstLogicalClusterForNewFile)
        {
            break;
        }
        else
        {
            if (IsFatEntryEndOfFile(CurrentFatEntry))
            {
                return 0;
            }
            else
            {
                CurrentLogicalCluster = CurrentFatEntry;
                CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);
            }
        }
    }

    return FirstLogicalClusterForNewFile;
}

u16 AddDirectoryToDirectory
(
    fat12_disk *Disk, u16 LogicalCluster, const char *DirectoryName
)
{
    u16 FirstLogicalClusterForNewDirectory = 0;

    u16 CurrentLogicalCluster = LogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetPhysicalCluster(Disk, CurrentLogicalCluster);
        FirstLogicalClusterForNewDirectory =
            AddDirectoryToDirectorySector(Disk, PhysicalSector, DirectoryName);

        if (FirstLogicalClusterForNewDirectory)
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

    return FirstLogicalClusterForNewDirectory;
}

u16 AddFileToRootDirectory(fat12_disk *Disk, ram_file File)
{
    u16 NewFileFirstLogicalCluster = 0;

    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
    {
        NewFileFirstLogicalCluster =
            AddFileToDirectorySector(Disk, &Disk->RootDirectory.Sectors[SectorIndex], File);

        if (NewFileFirstLogicalCluster)
        {
            break;
        }
    }

    return NewFileFirstLogicalCluster;
}

u16 AddDirectoryToRootDirectory(fat12_disk *Disk, const char *DirectoryName)
{
    u16 NewDirectoryFirstLogicalCluster = 0;

    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
    {
        NewDirectoryFirstLogicalCluster =
            AddDirectoryToDirectorySector
            (
                Disk, &Disk->RootDirectory.Sectors[SectorIndex], DirectoryName
            );

        if (NewDirectoryFirstLogicalCluster)
        {
            break;
        }
    }

    return NewDirectoryFirstLogicalCluster;
}