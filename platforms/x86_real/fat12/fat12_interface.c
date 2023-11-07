void Fat12InitializeRamDisk(disk_parameters *DiskParameters, void far *LoadAddress)
{
    u16 SectorsToLoad = 1 + (2 * FAT12_SECTORS_IN_FAT) + FAT12_SECTORS_IN_ROOT_DIRECTORY;
    ReadDiskSectors(DiskParameters, 0, SectorsToLoad, LoadAddress);
}

directory_entry far *
Fat12GetDirectoryEntryOfFile
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters *DiskParameters,
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

directory_entry far *
Fat12AddFile
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters *DiskParameters,
    char far *FullFilePath,
    void far *Memory,
    u32 Size
)
{
    // NOTE: there is a bug here
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
Fat12AddDirectory
(
    fat12_ram_disk far *Disk,
    memory_arena far *MemoryArena,
    disk_parameters *DiskParameters,
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
    disk_parameters *DiskParameters,
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
        directory_entry far *DirectoryEntry = Fat12GetDirectoryEntryOfFile
        (
            Disk, MemoryArena, DiskParameters, DirectoryPath
        );

        u16 CurrentClusterNumber = DirectoryEntry->ClusterNumberLowWord;
        u16 CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);

        for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
        {
            Fat12ListDirectorySector
            (
                GetSectorFromClusterNumber
                (
                    Disk,
                    MemoryArena,
                    DiskParameters,
                    CurrentClusterNumber
                )
            );

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
}