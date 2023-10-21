#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <math.h>
#include <strsafe.h>

#include "win32\base_types.h"
#include "shared\basic_defines.h"
#include "win32\file_system\files.h"
#include "win32\file_system\path_handling.h"
#include "win32\fat12\fat12.h"

#include "win32\math\scalar_conversions.cpp"
#include "win32\strings.cpp"
#include "win32\file_system\files.cpp"
#include "win32\file_system\path_handling.cpp"
#include "win32\fat12\fat12_get.cpp"
#include "win32\fat12\fat12_set.cpp"
#include "win32\fat12\fat12_interface.cpp"

i32 main(i32 argc, char **argv)
{
    char OutputDirectoryPath[1024] = {};
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    char BinaryFilePath[1024] = {};
    StringCchCatA(BinaryFilePath, ArrayCount(BinaryFilePath), OutputDirectoryPath);
    StringCchCatA(BinaryFilePath, ArrayCount(BinaryFilePath), "\\floppy.img");

    read_file_result DiskFile = ReadFileIntoMemory(BinaryFilePath);
    fat12_disk *Disk = (fat12_disk *)DiskFile.FileMemory;

    Fat12ListDirectory(Disk, "\\");

    return 0;
}