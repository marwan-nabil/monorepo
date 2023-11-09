#include "x86_real\shared\base_types.h"
#include "x86_real\shared\basic_defines.h"
#include "x86_real\shared\strings\strings.h"
#include "x86_real\shared\math\integers.h"
#include "x86_real\shared\memory\allocator.h"
#include "x86_real\shared\path_handling\path_handling.h"
#include "x86_real\shared\disk\disk.h"
#include "x86_real\shared\fat12\fat12.h"

#include "memory_layout.h"

#include "x86_real\shared\memory\memory.c"
#include "x86_real\shared\strings\strings.c"
#include "x86_real\shared\math\integers.c"
#include "x86_real\shared\memory\allocator.c"
#include "x86_real\shared\path_handling\path_handling.c"
#include "x86_real\shared\timing.c"
#include "x86_real\shared\disk\disk.c"
#include "x86_real\shared\fat12\fat12_get.c"
#include "x86_real\shared\fat12\fat12_set.c"
#include "x86_real\shared\fat12\fat12_interface.c"

#include "tests.c"

void _cdecl cstart(u16 BootDriveNumber)
{
    PrintString("\r\nHello world from C!\r\n");

    // StringAndMemoryUtilsTests();

    // DiskDriverTests(BootDriveNumber);

    // AllocatorTests(BootDriveNumber);

    // PathHandlingTests(BootDriveNumber);

    FileSystemTests(BootDriveNumber);

    while (1) {};
}