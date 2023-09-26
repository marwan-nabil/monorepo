void ListRootDirectory(fat12_disk *Disk)
{
    printf("\nlist root directory contents:\n");

    root_directory *RootDirectory = &Disk->RootDirectory;

    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(RootDirectory->Sectors); SectorIndex++)
    {
        sector *Sector = &RootDirectory->Sectors[SectorIndex];

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
                // printf("    > empty entry.\n");
            }
            else if (DirectoryEntry->FileName[0] == FILENAME_DELETED_SLOT)
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
}

ram_file CreateDummyFile(char *Name, char *Extension, u32 Size, u32 FillPattern)
{
    Assert(StringLength(Name) == 8);
    Assert(StringLength(Extension) == 3);

    ram_file Result = {};

    Result.Memory = (char *)malloc(Size);
    memset(Result.Memory, FillPattern, Size);

    Result.Size = Size;
    Result.Name = Name;
    Result.Extension = Extension;

    return Result;
}

b32
TestFat12()
{
    Assert(sizeof(boot_sector) == 512);
    Assert(sizeof(directory_entry) == 32);
    Assert(sizeof(sector) == 512);
    Assert(sizeof(file_allocation_table) == 4608);
    Assert(sizeof(root_directory) == (14 * FAT12_DISK_SECTOR_SIZE));
    Assert(sizeof(data_area) == (2847 * FAT12_DISK_SECTOR_SIZE));
    Assert(sizeof(fat12_disk) == (2880 * FAT12_DISK_SECTOR_SIZE));

    fat12_disk *Disk = (fat12_disk *)malloc(sizeof(fat12_disk));
    ZeroMemory(Disk, sizeof(fat12_disk));

    ram_file RootFile = CreateDummyFile("simple  ", "txt", 600, 0xFFFFFFFF);
    ram_file RootFile2 = CreateDummyFile("simple2 ", "txt", 1500, 0xFFFFFFFF);
    ram_file RootFile3 = CreateDummyFile("simple3 ", "txt", 1500, 0xFFFFFFFF);

    AddFileToRootDirectory(Disk, RootFile);
    AddFileToRootDirectory(Disk, RootFile2);
    AddFileToRootDirectory(Disk, RootFile3);

    ListRootDirectory(Disk);

    AddDirectoryToRootDirectory(Disk, "folder00");
    AddDirectoryToRootDirectory(Disk, "folder01");

    ListRootDirectory(Disk);


    printf("\nFinished.\n");
    free(Disk);
    free(RootFile.Memory);

    return TRUE;
}