#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>

#include "..\..\platform\base_types.h"
#include "..\..\platform\basic_defines.h"

#include "fat12.h"

#include "..\..\platform\console\console.cpp"
#include "..\..\platform\strings\strings.cpp"

#include "fat12_get.cpp"
#include "fat12_set.cpp"
#include "fat12_access_layer.cpp"

struct ram_file
{
    void *Memory;
    char Name[8];
    char Extension[3];
    u32 Size;
};

ram_file CreateDummyFile(char *Name, char *Extension, u32 Size, u32 FillPattern)
{
    ram_file Result = {};

    Result.Size = Size;
    Result.Memory = (char *)malloc(Size);
    memset(Result.Memory, FillPattern, Size);

    char LocalName[9] = {};
    char LocalExtension[4] = {};

    StringCchCat(LocalName, 9, Name);
    StringCchCat(LocalExtension, 4, Extension);

    memcpy(Result.Name, LocalName, 8);
    memcpy(Result.Extension, LocalExtension, 3);

    return Result;
}

i32 main(i32 argc, char **argv)
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

    ram_file File1 = CreateDummyFile("File1", "txt", 600, 0xFFFFFFFF);
    ram_file File2 = CreateDummyFile("File2", "txt", 1500, 0xFFFFFFFF);
    ram_file File3 = CreateDummyFile("File3", "txt", 2800, 0xFFFFFFFF);
    ram_file File4 = CreateDummyFile("File4", "txt", 40, 0xFFFFFFFF);
    ram_file File5 = CreateDummyFile("File5", "txt", 400, 0xFFFFFFFF);
    ram_file File6 = CreateDummyFile("File6", "txt", 8000, 0xFFFFFFFF);

    u16 File1Cluster = AddFileToRootDirectory(Disk, File1.Memory, File1.Size, File1.Name, File1.Extension);
    u16 File2Cluster = AddFileToRootDirectory(Disk, File2.Memory, File2.Size, File2.Name, File2.Extension);

    u16 Folder1Cluster = AddDirectoryToRootDirectory(Disk, "folder1");

    ListRootDirectory(Disk);

    u16 File3Cluster = AddFileToDirectory(Disk, Folder1Cluster, File3.Memory, File3.Size, File3.Name, File3.Extension);
    u16 SubDir0Cluster = AddDirectoryToDirectory(Disk, Folder1Cluster, "SubDir0");

    ListDirectory(Disk, Folder1Cluster);

    CreateFilePathSegmentList("\\aaaa\\bbbb\\cccc\\ddddd");

    directory_entry *File3DirectoryEntry = GetDirectoryEntryOfFileByPath(Disk, "\\folder1\\File3.txt");

    free(Disk);
    free(File1.Memory);
    free(File2.Memory);
    free(File3.Memory);
    free(File4.Memory);
    free(File5.Memory);
    free(File6.Memory);

    printf("\nFinished.\n");

    return TRUE;
}