#include "libraries\x86\base_types.h"
#include "libraries\x86\basic_defines.h"
#include "libraries\x86\bios\strings.h"
#include "libraries\x86\bios\disk.h"
#include "libraries\x86\strings\strings.h"
#include "libraries\x86\math\integers.h"
#include "libraries\x86\memory\arena_allocator.h"
#include "libraries\x86\path_handling\path_handling.h"
#include "libraries\x86\disk\disk.h"
#include "libraries\x86\fat12\fat12.h"
#include "libraries\x86\file_io\file_io.h"

#include "projects\os\bootloader\memory_layout.h"

#include "libraries\x86\memory\memory.c"
#include "libraries\x86\strings\strings.c"
#include "libraries\x86\math\integers.c"
#include "libraries\x86\memory\arena_allocator.c"
#include "libraries\x86\path_handling\path_handling.c"
#include "libraries\x86\timing.c"
#include "libraries\x86\disk\disk.c"
#include "libraries\x86\fat12\get.c"
#include "libraries\x86\fat12\set.c"
#include "libraries\x86\file_io\file_io.c"

#include "projects\os\bootloader\tests.c"

void __attribute__((cdecl)) cstart(u16 BootDriveNumber)
{
    // PrintString("\r\nHello world from C!\r\n");

    // StringAndMemoryUtilsTests();

    // DiskDriverTests(BootDriveNumber);

    // AllocatorTests(BootDriveNumber);

    // PathHandlingTests(BootDriveNumber);

    // Fat12Tests(BootDriveNumber);

    // FileIoTests(BootDriveNumber);

    while (1) {};
}