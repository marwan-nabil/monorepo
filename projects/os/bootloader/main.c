#include <stdarg.h>

#include "libraries\x86\base_types.h"
#include "libraries\x86\basic_defines.h"
#include "libraries\x86\vga\vga.h"
#include "libraries\x86\bios\strings.h"
#include "libraries\x86\bios\disk.h"
#include "libraries\x86\io\io.h"
#include "libraries\x86\math\integers.h"
#include "libraries\x86\strings\strings.h"
#include "libraries\x86\disk\disk.h"
#include "libraries\x86\fat12\fat12.h"
#include "libraries\x86\path_handling\path_handling.h"
#include "libraries\x86\memory\arena_allocator.h"
#include "libraries\x86\file_io\file_io.h"

#include "libraries\x86\vga\vga.c"
#include "libraries\x86\timing.c"
#include "libraries\x86\memory\memory.c"
#include "libraries\x86\strings\strings.c"
#include "libraries\x86\disk\disk.c"
#include "libraries\x86\memory\arena_allocator.c"
#include "libraries\x86\path_handling\path_handling.c"
#include "libraries\x86\fat12\get.c"
#include "libraries\x86\fat12\set.c"
#include "libraries\x86\file_io\file_io.c"
#include "projects\os\bootloader\tests.c"

u8 FreeStore[KiloBytes(64)];

void __attribute__((cdecl)) cstart(u32 BootDriveNumber)
{
    TestVGA();
    // TestIO();
    // TestBIOSFunctions(BootDriveNumber);
    // StringTests();
    // DiskDriverTests(BootDriveNumber, FreeStore);
    // AllocatorTests(FreeStore);
    // PathHandlingTests(FreeStore);
    // Fat12Tests(BootDriveNumber, FreeStore);
    // FileIoTests(BootDriveNumber, FreeStore);

    while (1) {};
}