#include "x86_bios\shared\base_types.h"
#include "x86_bios\shared\basic_defines.h"
#include "x86_bios\shared\strings\strings.h"
#include "x86_bios\shared\math\integers.h"
#include "x86_bios\shared\memory\arena_allocator.h"
#include "x86_bios\shared\path_handling\path_handling.h"
#include "x86_bios\shared\disk\disk.h"
#include "x86_bios\shared\fat12\fat12.h"
#include "x86_bios\shared\file_io\file_io.h"

#include "memory_layout.h"

#include "x86_bios\shared\memory\memory.c"
#include "x86_bios\shared\strings\strings.c"
#include "x86_bios\shared\math\integers.c"
#include "x86_bios\shared\memory\arena_allocator.c"
#include "x86_bios\shared\path_handling\path_handling.c"
#include "x86_bios\shared\timing.c"
#include "x86_bios\shared\disk\disk.c"
#include "x86_bios\shared\fat12\fat12_get.c"
#include "x86_bios\shared\fat12\fat12_set.c"
#include "x86_bios\shared\fat12\fat12_interface.c"
#include "x86_bios\shared\file_io\file_io.c"

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