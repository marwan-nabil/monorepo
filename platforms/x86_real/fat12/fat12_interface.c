void Fat12LoadDiskIntoRam(disk_parameters *DiskParameters, void far *LoadAddress)
{
    u16 SectorsToLoad = 1 + (2 * FAT12_SECTORS_IN_FAT) + FAT12_SECTORS_IN_ROOT_DIRECTORY;
    ReadDiskSectors(DiskParameters, 0, SectorsToLoad, LoadAddress);
}

// ====================================================================
// directory_entry far *
// Fat12GetDirectoryEntryOfFile(fat12_ram_disk far *Disk, char far *FullFilePath)
// {
//     if (StringLengthFar(FullFilePath) == 1)
//     {
//         return NULL;
//     }

//     file_path_node *FirstNode = CreateFilePathSegmentList(FullFilePath);
//     if (!FirstNode)
//     {
//         return NULL;
//     }

//     file_path_node *CurrentNode = FirstNode;

//     char LocalFileName[8] = {};
//     char LocalFileExtension[3] = {};

//     GetFileNameAndExtensionFromString
//     (
//         CurrentNode->FileName, LocalFileName, 8, LocalFileExtension, 3
//     );

//     directory_entry *CurrentEntry = GetDirectoryEntryOfFileInRootDirectory
//     (
//         Disk,
//         LocalFileName,
//         LocalFileExtension
//     );

//     if (CurrentEntry && !CurrentNode->ChildNode)
//     {
//         return CurrentEntry;
//     }

//     CurrentNode = CurrentNode->ChildNode;

//     while (CurrentNode)
//     {
//         ZeroMemory(LocalFileName, ArrayCount(LocalFileName));
//         ZeroMemory(LocalFileExtension, ArrayCount(LocalFileExtension));

//         GetFileNameAndExtensionFromString
//         (
//             CurrentNode->FileName, LocalFileName, 8, LocalFileExtension, 3
//         );

//         CurrentEntry = GetDirectoryEntryOfFileInDirectory
//         (
//             Disk,
//             CurrentEntry,
//             LocalFileName,
//             LocalFileExtension
//         );

//         if (CurrentEntry && !CurrentNode->ChildNode)
//         {
//             FreeFilePathSegmentList(FirstNode);
//             return CurrentEntry;
//         }

//         CurrentNode = CurrentNode->ChildNode;
//     }

//     FreeFilePathSegmentList(FirstNode);
//     return NULL;
// }
// ====================================================================

static void Fat12ListDirectorySector(sector far *Sector)
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

void Fat12ListDirectory(fat12_ram_disk far *Disk, char far *DirectoryPath)
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
        // directory_entry far *DirectoryEntry = Fat12GetDirectoryEntryOfFile(Disk, DirectoryPath);

        // u16 CurrentClusterNumber = DirectoryEntry->ClusterNumberLowWord;
        // u16 CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);

        // for (u32 LoopIndex = 0; LoopIndex < FAT12_SECTORS_IN_DATA_AREA; LoopIndex++)
        // {
        //     Fat12ListDirectorySector(GetSectorFromClusterNumber(Disk, CurrentClusterNumber));

        //     if (IsFatEntryEndOfFile(CurrentFatEntry))
        //     {
        //         break;
        //     }
        //     else
        //     {
        //         CurrentClusterNumber = CurrentFatEntry;
        //         CurrentFatEntry = GetFatEntry(Disk, CurrentClusterNumber);
        //     }
        // }
    }
}