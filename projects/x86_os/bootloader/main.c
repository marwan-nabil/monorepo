#include "platforms\x86_real\base_types.h"
#include "platforms\x86_real\basic_defines.h"
#include "platforms\x86_real\console\strings.h"
#include "platforms\x86_real\math\integers.h"
#include "platforms\x86_real\memory\allocator.h"
#include "platforms\x86_real\disk\disk.h"
#include "platforms\x86_real\fat12\fat12.h"

#include "memory_layout.h"

#include "platforms\x86_real\memory\memory.c"
#include "platforms\x86_real\console\characters.c"
#include "platforms\x86_real\console\strings.c"
#include "platforms\x86_real\math\integers.c"
#include "platforms\x86_real\memory\allocator.c"
#include "platforms\x86_real\disk\disk.c"
#include "platforms\x86_real\fat12\fat12_get.c"
#include "platforms\x86_real\fat12\fat12_set.c"
#include "platforms\x86_real\fat12\fat12_interface.c"

#include "tests.c"

void _cdecl cstart(u16 BootDrive)
{
    PrintString("\r\nHello world from C!\r\n");

    StringAndMemoryUtilsTests();

    DiskDriverTests();

    FileSystemTests();

    AllocatorTests();

    while (1) {};
}