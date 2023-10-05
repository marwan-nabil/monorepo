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
    char LocalPathBuffer[FAT12_MAX_PATH] = {};
    StringCchCat(LocalPathBuffer, ArrayCount(LocalPathBuffer), FileFullPath);

    u32 PathLength = StringLength(LocalPathBuffer);

    file_path_node *CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
    *CurrentFilePathNode = {};
    file_path_node *LastFilePathNode = 0;

    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (LocalPathBuffer[CharIndex] == '\\')
        {
            char PathSegment[FAT12_MAX_PATH] = {};
            StringCchCat(PathSegment, FAT12_MAX_PATH, &LocalPathBuffer[CharIndex + 1]);
            ZeroMemory(&LocalPathBuffer[CharIndex], StringLength(&LocalPathBuffer[CharIndex]));

            if (!CurrentFilePathNode)
            {
                CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
                *CurrentFilePathNode = {};
            }

            char LocalFileName[8] = {};
            char LocalFileExtension[3] = {};

            GetFileNameAndExtensionFromString
            (
                PathSegment, LocalFileName, 8, LocalFileExtension, 3
            );

            memcpy(CurrentFilePathNode->FileName, LocalFileName, 8);
            memcpy(CurrentFilePathNode->FileExtension, LocalFileExtension, 3);
            CurrentFilePathNode->ChildNode = LastFilePathNode;

            LastFilePathNode = CurrentFilePathNode;
            CurrentFilePathNode = 0;
        }
    }

    return LastFilePathNode;
}

inline directory_entry *
GetDirectoryEntryOfFileByPath(fat12_disk *Disk, char *FullFilePath)
{
    file_path_node *CurrentNode = CreateFilePathSegmentList(FullFilePath);
    directory_entry *CurrentEntry =
        GetDirectoryEntryOfFileInRootDirectory(Disk, CurrentNode->FileName, CurrentNode->FileExtension);

    if (CurrentEntry && !CurrentNode->ChildNode)
    {
        return CurrentEntry;
    }

    CurrentNode = CurrentNode->ChildNode;

    while (CurrentNode)
    {
        CurrentEntry =
            GetDirectoryEntryOfFileInDirectory
            (
                Disk,
                CurrentEntry->FirstLogicalCluster,
                CurrentNode->FileName,
                CurrentNode->FileExtension
            );

        if (CurrentEntry && !CurrentNode->ChildNode)
        {
            return CurrentEntry;
        }

        CurrentNode = CurrentNode->ChildNode;
    }

    return NULL;
}

void ListDirectorySector(sector *Sector)
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
            // printf("    > empty entry.\n");
        }
        else if (DirectoryEntry->FileName[0] == FAT12_FILENAME_DELETED_SLOT)
        {
            printf("    > deleted file.\n");
        }
        else
        {
            if (DirectoryEntry->FileAttributes == FAT12_FILE_ATTRIBUTE_NORMAL)
            {
                char FileNameString[13];

                memcpy(FileNameString, DirectoryEntry->FileName, 8);
                memcpy(FileNameString + 8, (void *)".", 1);
                memcpy(FileNameString + 9, DirectoryEntry->FileExtension, 3);
                FileNameString[12] = 0;
                printf("    FILE:   %s\n", FileNameString);
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

void ListDirectory(fat12_disk *Disk, u16 LogicalCluster)
{
    printf("\nlist directory contents:\n");

    u16 CurrentLogicalCluster = LogicalCluster;
    u16 CurrentFatEntry = GetFatEntry(Disk, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetSector(Disk, CurrentLogicalCluster);
        ListDirectorySector(PhysicalSector);

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

void ListRootDirectory(fat12_disk *Disk)
{
    printf("\nlist root directory contents:\n");

    root_directory *RootDirectory = &Disk->RootDirectory;

    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(RootDirectory->Sectors); SectorIndex++)
    {
        sector *Sector = &RootDirectory->Sectors[SectorIndex];
        ListDirectorySector(Sector);
    }
}

fat12_disk *Fat12CreateRamDisk()
{
    fat12_disk *Disk = (fat12_disk *)VirtualAlloc
    (
        0, sizeof(fat12_disk), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
    );
    // TODO_LATER: investigate why *Disk = {}; causes a stack overflow
    memset(Disk, 0, sizeof(fat12_disk));
    return Disk;
}