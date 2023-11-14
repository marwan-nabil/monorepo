#include "x86\shared\base_types.h"
#include "x86\shared\basic_defines.h"
#include "x86\shared\strings\strings.h"
#include "x86\shared\math\integers.h"
#include "x86\shared\memory\arena_allocator.h"
#include "x86\shared\path_handling\path_handling.h"
#include "x86\shared\disk\disk.h"
#include "x86\shared\fat12\fat12.h"
#include "x86\shared\file_io\file_io.h"

#include "memory_layout.h"

#include "x86\shared\memory\memory.c"
#include "x86\shared\strings\strings.c"
#include "x86\shared\math\integers.c"
#include "x86\shared\memory\arena_allocator.c"
#include "x86\shared\path_handling\path_handling.c"
#include "x86\shared\timing.c"
#include "x86\shared\disk\disk.c"
#include "x86\shared\fat12\get.c"
#include "x86\shared\fat12\set.c"
#include "x86\shared\file_io\file_io.c"

#include "tests.c"

void _cdecl cstart(u16 BootDriveNumber)
{
    PrintString("\r\nHello world from C!\r\n");

    // StringAndMemoryUtilsTests();

    // DiskDriverTests(BootDriveNumber);

    // AllocatorTests(BootDriveNumber);

    // PathHandlingTests(BootDriveNumber);

    // Fat12Tests(BootDriveNumber);

    FileIoTests(BootDriveNumber);

    while (1) {};
}