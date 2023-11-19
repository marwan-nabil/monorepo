#include "libraries\x86_real\base_types.h"
#include "libraries\x86_real\basic_defines.h"
#include "libraries\x86_real\bios\strings.h"
#include "libraries\x86_real\bios\disk.h"
#include "libraries\x86_real\strings\strings.h"
#include "libraries\x86_real\math\integers.h"
#include "libraries\x86_real\memory\arena_allocator.h"
#include "libraries\x86_real\path_handling\path_handling.h"
#include "libraries\x86_real\disk\disk.h"
#include "libraries\x86_real\fat12\fat12.h"
#include "libraries\x86_real\file_io\file_io.h"

#include "projects\os_real\bootloader\memory_layout.h"

#include "libraries\x86_real\memory\memory.c"
#include "libraries\x86_real\strings\strings.c"
#include "libraries\x86_real\math\integers.c"
#include "libraries\x86_real\memory\arena_allocator.c"
#include "libraries\x86_real\path_handling\path_handling.c"
#include "libraries\x86_real\timing.c"
#include "libraries\x86_real\disk\disk.c"
#include "libraries\x86_real\fat12\get.c"
#include "libraries\x86_real\fat12\set.c"
#include "libraries\x86_real\file_io\file_io.c"

#include "projects\os_real\bootloader\tests.c"

void __attribute__((cdecl)) cstart(u16 BootDriveNumber)
{
    PrintString("\r\nHello world from C!\r\n");

    // StringAndMemoryUtilsTests();

    // DiskDriverTests(BootDriveNumber);

    // AllocatorTests(BootDriveNumber);

    // PathHandlingTests(BootDriveNumber);

    // Fat12Tests(BootDriveNumber);

    // FileIoTests(BootDriveNumber);

    while (1) {};
}