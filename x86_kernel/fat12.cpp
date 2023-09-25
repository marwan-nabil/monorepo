#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\assertions.h"
#include "..\miscellaneous\basic_defines.h"
#include "..\miscellaneous\strings.cpp"

#include "fat12.h"

inline u16 TranslateCluster(u16 LogicalCluster)
{
    Assert(LogicalCluster >= 2);
    u16 PhysicalCluster = 33 + LogicalCluster - 2;
    return PhysicalCluster;
}

inline u16 GetFatEntry(file_allocation_table *Fat, u32 LogicalCluster)
{
    u16 Result = 0;
    u32 StartingByteIndex = LogicalCluster * 3 / 2;

    // ## #@ @@ %% %& && $$ $# ## @@ @  ...
    // DD DD DD DD DD DD DD DD DD DD DD ...

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

    // ## #@ @@ %% %& && $$ $# ## @@ @  ...
    // DD DD DD DD DD DD DD DD DD DD DD ...

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

u16 GetFirstEmptyLogicalCluster(fat12_disk *Disk)
{
    for (u16 LogicalCluster = 2; LogicalCluster < FAT12_ENTRIES_PER_FAT; LogicalCluster++)
    {
        u16 FatEntry = GetFatEntry(&Disk->Fat1, LogicalCluster);
        if (FatEntry == FAT_ENTRY_FREE_CLUSTER)
        {
            return LogicalCluster;
        }
    }

    printf("ERROR: could not find a single empty logical cluster.\n");

    return 0;
}

directory_entry *GetFirstFreeDirectoryEntry(sector *Sector)
{
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
            return DirectoryEntry;
        }
    }

    return NULL;
}

void ListRootDirectoryFiles(fat12_disk *Disk)
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
                char FileNameString[13];
                ZeroMemory(FileNameString, ArrayCount(FileNameString));
                memcpy(FileNameString, DirectoryEntry->FileName, 8);
                StringCchCatA(FileNameString, ArrayCount(FileNameString), ".");
                memcpy(FileNameString + 9, DirectoryEntry->FileExtension, 3);
                printf("    > %s\n", FileNameString);
            }
        }
    }
}

create_file_result CreateDummyFile(char *Name, char *Extension, u32 Size, u32 FillPattern)
{
    Assert(StringLength(Name) == 8);
    Assert(StringLength(Extension) == 3);

    create_file_result Result = {};

    Result.Memory = (char *)malloc(Size);
    memset(Result.Memory, FillPattern, Size);

    Result.Size = Size;
    Result.Name = Name;
    Result.Extension = Extension;

    return Result;
}

u16 AllocateFileSectors(fat12_disk *Disk, create_file_result File)
{
    u16 FileFirstLogicalCluster = 0;

    u32 ClustersNeeded = (File.Size + FAT12_DISK_CLUSTER_SIZE) / FAT12_DISK_CLUSTER_SIZE;
    u32 SizeLeft = File.Size;
    char *ReadPointer = File.Memory;

    u16 PreviousLogicalCluster = 0;

    while (ClustersNeeded)
    {
        u16 LogicalCluster = GetFirstEmptyLogicalCluster(Disk);
        SetFatEntry(&Disk->Fat1, LogicalCluster, FAT_ENTRY_RESERVED_CLUSTER);

        if (PreviousLogicalCluster)
        {
            SetFatEntry(&Disk->Fat1, PreviousLogicalCluster, LogicalCluster);
        }
        else
        {
            FileFirstLogicalCluster = LogicalCluster;
        }

        u16 PhysicalCluster = TranslateCluster(LogicalCluster);

        u32 BytesToCopy = 512;
        u32 BytesToZero = 0;

        if (SizeLeft < 512)
        {
            BytesToCopy = SizeLeft;
            BytesToZero = 512 - SizeLeft;
        }

        memcpy(Disk->Sectors[PhysicalCluster].Bytes, ReadPointer, BytesToCopy);
        ZeroMemory(Disk->Sectors[PhysicalCluster].Bytes + BytesToCopy, BytesToZero);

        ReadPointer += BytesToCopy;
        SizeLeft -= BytesToCopy;
        ClustersNeeded--;

        PreviousLogicalCluster = LogicalCluster;

        if (SizeLeft == 0)
        {
            SetFatEntry(&Disk->Fat1, LogicalCluster, FAT_ENTRY_END_OF_FILE_CLUSTER_RANGE_END);
        }
    }

    if (!FileFirstLogicalCluster)
    {
        printf("ERROR: Cannot allocate file into sectors\n");
    }

    return FileFirstLogicalCluster;
}

b32 AddFileToDirectorySector(sector *DirectorySector, create_file_result File)
{
    directory_entry *DirectoryEntry = 
        GetFirstFreeDirectoryEntry(DirectorySector);

    if (!DirectoryEntry)
    {
        return FALSE;
    }

    memcpy(DirectoryEntry->FileName, File.Name, 8);
    memcpy(DirectoryEntry->FileExtension, File.Extension, 3);
    DirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_NORMAL;
    DirectoryEntry->FileSize = File.Size;
    DirectoryEntry->FirstLogicalCluster = AllocateFileSectors(Disk, File);

    return TRUE;
}

void AddFileToRootDirectory(fat12_disk *Disk, create_file_result File)
{
    directory_entry *FreeDirectoryEntry = NULL;
    for (u32 SectorIndex = 0; SectorIndex < ArrayCount(Disk->RootDirectory.Sectors); SectorIndex++)
    {
        FreeDirectoryEntry =
            GetFirstFreeDirectoryEntry(&Disk->RootDirectory.Sectors[SectorIndex]);

        if (FreeDirectoryEntry)
        {
            break;
        }
    }

    if (!FreeDirectoryEntry)
    {
        printf("ERROR: no free directory entries left in root directory of the disk.\n");
        return;
    }

    memcpy(FreeDirectoryEntry->FileName, File.Name, 8);
    memcpy(FreeDirectoryEntry->FileExtension, File.Extension, 3);
    FreeDirectoryEntry->FileAttributes = FAT12_FILE_ATTRIBUTE_NORMAL;
    FreeDirectoryEntry->FileSize = File.Size;
    FreeDirectoryEntry->FirstLogicalCluster = AllocateFileSectors(Disk, File);
}

i32
main(i32 argc, u8 **argv)
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

    create_file_result RootFile = CreateDummyFile("simple  ", "txt", 600, 0xFFFFFFFF);
    create_file_result RootFile2 = CreateDummyFile("simple2 ", "txt", 1500, 0xFFFFFFFF);
    AddFileToRootDirectory(Disk, RootFile);
    AddFileToRootDirectory(Disk, RootFile2);

    ListRootDirectoryFiles(Disk);




    printf("\nFinished.\n");
    free(Disk);
    free(RootFile.Memory);

    return 0;
}