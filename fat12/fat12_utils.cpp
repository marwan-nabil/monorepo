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
    u16 CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);

    while (1)
    {
        sector *PhysicalSector = GetPhysicalCluster(Disk, CurrentLogicalCluster);
        ListDirectorySector(PhysicalSector);

        if (IsFatEntryEndOfFile(CurrentFatEntry))
        {
            break;
        }
        else
        {
            CurrentLogicalCluster = CurrentFatEntry;
            CurrentFatEntry = GetFatEntry(&Disk->Fat1, CurrentLogicalCluster);
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

ram_file CreateDummyFile(char *Name, char *Extension, u32 Size, u32 FillPattern)
{
    ram_file Result = {};

    Result.Size = Size;
    Result.Memory = (char *)malloc(Size);
    memset(Result.Memory, FillPattern, Size);

    StringCchCat(Result.Name, 8, Name);
    StringCchCat(Result.Extension, 3, Extension);

    return Result;
}