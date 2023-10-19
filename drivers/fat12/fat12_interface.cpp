static directory_entry *
Fat12GetDirectoryEntryOfFile(fat12_disk *Disk, char *FullFilePath)
{
    if (StringLength(FullFilePath) == 1)
    {
        return NULL;
    }

    file_path_node *FirstNode = CreateFilePathSegmentList(FullFilePath);
    if (!FirstNode)
    {
        return NULL;
    }

    file_path_node *CurrentNode = FirstNode;

    char LocalFileName[8] = {};
    char LocalFileExtension[3] = {};

    GetFileNameAndExtensionFromString
    (
        CurrentNode->FileName, LocalFileName, 8, LocalFileExtension, 3
    );

    directory_entry *CurrentEntry = GetDirectoryEntryOfFileInRootDirectory
    (
        Disk,
        LocalFileName,
        LocalFileExtension
    );

    if (CurrentEntry && !CurrentNode->ChildNode)
    {
        return CurrentEntry;
    }

    CurrentNode = CurrentNode->ChildNode;

    while (CurrentNode)
    {
        ZeroMemory(LocalFileName, ArrayCount(LocalFileName));
        ZeroMemory(LocalFileExtension, ArrayCount(LocalFileExtension));

        GetFileNameAndExtensionFromString
        (
            CurrentNode->FileName, LocalFileName, 8, LocalFileExtension, 3
        );

        CurrentEntry = GetDirectoryEntryOfFileInDirectory
        (
            Disk,
            CurrentEntry,
            LocalFileName,
            LocalFileExtension
        );

        if (CurrentEntry && !CurrentNode->ChildNode)
        {
            FreeFilePathSegmentList(FirstNode);
            return CurrentEntry;
        }

        CurrentNode = CurrentNode->ChildNode;
    }

    FreeFilePathSegmentList(FirstNode);
    return NULL;
}

static directory_entry *
Fat12AddFile(fat12_disk *Disk, char *FullFilePath, void *Memory, u32 Size)
{
    if (StringLength(FullFilePath) == 1)
    {
        return NULL;
    }

    file_path_node *CurrentPathNode = CreateFilePathSegmentList(FullFilePath);

    char LocalFileName[8] = {};
    char LocalFileExtension[3] = {};
    GetFileNameAndExtensionFromString
    (
        CurrentPathNode->FileName, LocalFileName, 8, LocalFileExtension, 3
    );

    if (!CurrentPathNode->ChildNode)
    {
        directory_entry *FileDirectoryEntry = GetDirectoryEntryOfFileInRootDirectory
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
                Disk, LocalFileName, LocalFileExtension,
                Memory, Size
            );
            return FileDirectoryEntry;
        }
    }

    directory_entry *CurrentDirectoryEntry =
        GetDirectoryEntryOfDirectoryInRootDirectory(Disk, LocalFileName);
    CurrentPathNode = CurrentPathNode->ChildNode;

    while (CurrentPathNode)
    {
        ZeroMemory(LocalFileName, ArrayCount(LocalFileName));
        ZeroMemory(LocalFileExtension, ArrayCount(LocalFileExtension));

        GetFileNameAndExtensionFromString
        (
            CurrentPathNode->FileName, LocalFileName, 8, LocalFileExtension, 3
        );

        if (!CurrentPathNode->ChildNode)
        {
            directory_entry *FileDirectoryEntry = GetDirectoryEntryOfFileInDirectory
            (
                Disk,
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
                    Disk, CurrentDirectoryEntry,
                    LocalFileName, LocalFileExtension,
                    Memory, Size
                );
                return FileDirectoryEntry;
            }
        }
        else
        {
            CurrentDirectoryEntry =
                GetDirectoryEntryOfDirectoryInDirectory(Disk, CurrentDirectoryEntry, LocalFileName);
            CurrentPathNode = CurrentPathNode->ChildNode;
        }
    }

    return NULL;
}

static directory_entry *
Fat12AddDirectory(fat12_disk *Disk, char *DirectoryPath)
{
    if (StringLength(DirectoryPath) == 1)
    {
        return NULL;
    }

    file_path_node *CurrentPathNode = CreateFilePathSegmentList(DirectoryPath);

    if (!CurrentPathNode->ChildNode)
    {
        char LocalDirectoryName[8] = {};
        FillFixedSizeStringBuffer
        (
            LocalDirectoryName,
            ArrayCount(LocalDirectoryName),
            CurrentPathNode->FileName
        );

        directory_entry *DirectoryDirectoryEntry = GetDirectoryEntryOfDirectoryInRootDirectory
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

            directory_entry *DirectoryDirectoryEntry = AddDirectoryToRootDirectory
            (
                Disk, LocalDirectoryName
            );
            return DirectoryDirectoryEntry;
        }
    }

    directory_entry *CurrentDirectoryEntry =
        GetDirectoryEntryOfDirectoryInRootDirectory(Disk, CurrentPathNode->FileName);
    CurrentPathNode = CurrentPathNode->ChildNode;

    while (CurrentPathNode)
    {
        if (!CurrentPathNode->ChildNode)
        {
            char LocalDirectoryName[8] = {};
            FillFixedSizeStringBuffer
            (
                LocalDirectoryName,
                ArrayCount(LocalDirectoryName),
                CurrentPathNode->FileName
            );

            directory_entry *DirectoryDirectoryEntry =
                GetDirectoryEntryOfDirectoryInDirectory
                (
                    Disk,
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
                    Disk, CurrentDirectoryEntry, LocalDirectoryName
                );
                return DirectoryDirectoryEntry;
            }
        }
        else
        {
            CurrentDirectoryEntry = GetDirectoryEntryOfDirectoryInDirectory
            (
                Disk, CurrentDirectoryEntry, CurrentPathNode->FileName
            );
            CurrentPathNode = CurrentPathNode->ChildNode;
        }
    }

    return NULL;
}

static void Fat12ListDirectorySector(sector *Sector)
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
        }
        else if (DirectoryEntry->FileName[0] == FAT12_FILENAME_DELETED_SLOT)
        {
            printf("    > deleted file.\n");
        }
        else
        {
            if (DirectoryEntry->FileAttributes == FAT12_FILE_ATTRIBUTE_NORMAL)
            {
                char FileNameString[9];
                char FileExtensionString[4];

                memcpy(FileNameString, DirectoryEntry->FileName, 8);
                memcpy(FileExtensionString, DirectoryEntry->FileExtension, 3);

                FileNameString[8] = 0;
                FileExtensionString[3] = 0;

                printf("    FILE:   %s.%s\n", FileNameString, FileExtensionString);
            }
            else if (DirectoryEntry->FileAttributes == FAT12_FILE_ATTRIBUTE_DIRECTORY)
            {
                char FileNameString[9];

                memcpy(FileNameString, DirectoryEntry->FileName, 8);
                FileNameString[8] = 0;
                printf("     DIR:   %s\n", FileNameString);
            }
        }
    }
}

static void Fat12ListDirectory(fat12_disk *Disk, char *DirectoryPath)
{
    printf("\nlist %s:\n", DirectoryPath);

    if
    (
        (StringLength(DirectoryPath) == 1) &&
        (memcmp(DirectoryPath, "\\", 1) == 0)
    )
    {
        for (u32 SectorIndex = 0; SectorIndex < FAT12_SECTORS_IN_ROOT_DIRECTORY; SectorIndex++)
        {
            Fat12ListDirectorySector(&Disk->RootDirectory.Sectors[SectorIndex]);
        }
    }
    else
    {
        directory_entry *DirectoryEntry = Fat12GetDirectoryEntryOfFile(Disk, DirectoryPath);

        u16 CurrentClusterNumber = DirectoryEntry->ClusterNumberLowWord;
        u16 CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);

        for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
        {
            Fat12ListDirectorySector(GetSectorFromClusterNumber(Disk, CurrentClusterNumber));

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