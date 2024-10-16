#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>

#include "platforms\win32\libraries\base_types.h"
#include "platforms\win32\libraries\basic_defines.h"
#include "platforms\win32\libraries\shell\console.h"
#include "platforms\win32\libraries\strings\path_handling.h"
#include "platforms\win32\libraries\file_system\fat12\fat12.h"
#include "platforms\win32\libraries\file_system\fat12\fat12_interface.h"

struct ram_file
{
    void *Memory;
    char FullPath[MAX_PATH];
    u32 Size;
};

ram_file CreateDummyFile(char *FullFilePath, u32 Size, u32 FillPattern)
{
    ram_file Result = {};

    Result.Size = Size;
    Result.Memory = (char *)malloc(Size);
    memset(Result.Memory, FillPattern, Size);

    char LocalPath[MAX_PATH] = {};
    StringCchCat(LocalPath, MAX_PATH, FullFilePath);
    memcpy(Result.FullPath, LocalPath, MAX_PATH);

    return Result;
}

i32 main(i32 argc, char **argv)
{
    Assert(sizeof(boot_sector) == 512);
    Assert(sizeof(directory_entry) == 32);
    Assert(sizeof(sector) == 512);
    Assert(sizeof(file_allocation_table) == 4608);
    Assert(sizeof(root_directory) == (14 * FAT12_SECTOR_SIZE));
    Assert(sizeof(data_area) == (2847 * FAT12_SECTOR_SIZE));
    Assert(sizeof(fat12_disk) == (2880 * FAT12_SECTOR_SIZE));

    InitializeConsole();

    fat12_disk *Disk = (fat12_disk *)malloc(sizeof(fat12_disk));
    ZeroMemory(Disk, sizeof(fat12_disk));

    ram_file File1 = CreateDummyFile("\\File1.txt", 600, 0xFFFFFFFF);
    ram_file File2 = CreateDummyFile("\\Folder1\\File2.txt", 1500, 0xFFFFFFFF);
    ram_file File3 = CreateDummyFile("\\File3.txt", 600, 0xFFFFFFFF);
    ram_file File4 = CreateDummyFile("\\Folder1\\Sub1\\File4.txt", 8000, 0xFFFFFFFF);

    // add files & folders in the root folder of the disk
    Fat12AddFile(Disk, File1.FullPath, File1.Memory, File1.Size);
    Fat12AddFile(Disk, File3.FullPath, File3.Memory, File3.Size);
    Fat12AddDirectory(Disk, "\\Folder1");
    Fat12AddDirectory(Disk, "\\Folder2");

    // add files and folders in \folder1
    Fat12AddFile(Disk, File2.FullPath, File2.Memory, File2.Size);
    Fat12AddDirectory(Disk, "\\Folder1\\Sub1");

    // add files and folders in \folder1\Sub1
    Fat12AddFile(Disk, File4.FullPath, File4.Memory, File4.Size);

    Fat12ListDirectory(Disk, "\\");
    Fat12ListDirectory(Disk, "\\Folder1");
    Fat12ListDirectory(Disk, "\\Folder1\\Sub1");

    free(Disk);
    free(File1.Memory);
    free(File2.Memory);

    ConsolePrintColored("\nFinished.\n", FOREGROUND_GREEN);

    return 0;
}