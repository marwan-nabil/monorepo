#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <math.h>
#include <strsafe.h>

#include "..\..\platform\base_types.h"
#include "..\..\platform\basic_defines.h"
#include "..\..\platform\file_system\files.h"
#include "..\..\drivers\fat12\fat12.h"

#include "..\..\math\scalar_conversions.cpp"
#include "..\..\platform\strings\strings.cpp"
#include "..\..\platform\file_system\files.cpp"
#include "..\..\drivers\fat12\fat12_get.cpp"
#include "..\..\drivers\fat12\fat12_set.cpp"
#include "..\..\drivers\fat12\fat12_access_layer.cpp"

i32 main(i32 argc, char **argv)
{
    char OutputDirectoryPath[1024];
    ZeroMemory(OutputDirectoryPath, ArrayCount(OutputDirectoryPath));
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    char BinaryFilePath[1024];
    ZeroMemory(BinaryFilePath, ArrayCount(BinaryFilePath));
    StringCchCatA(BinaryFilePath, ArrayCount(BinaryFilePath), OutputDirectoryPath);
    StringCchCatA(BinaryFilePath, ArrayCount(BinaryFilePath), "\\floppy.img");

    read_file_result DiskFile = ReadFileIntoMemory(BinaryFilePath);
    fat12_disk *Disk = (fat12_disk *)DiskFile.FileMemory;

    ListRootDirectory(Disk);

    return 0;
}