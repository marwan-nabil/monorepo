#include <stdarg.h>

#include "libraries\x86\base_types.h"
#include "libraries\x86\basic_defines.h"
#include "libraries\x86\vga\vga.h"
#include "libraries\x86\bios\strings.h"
#include "libraries\x86\bios\disk.h"
#include "libraries\x86\math\integers.h"
#include "libraries\x86\strings\strings.h"

#include "libraries\x86\vga\vga.c"
#include "libraries\x86\timing.c"
#include "libraries\x86\memory\memory.c"
#include "libraries\x86\strings\strings.c"
#include "projects\os\bootloader\tests.c"

void __attribute__((cdecl)) cstart(u32 BootDriveNumber)
{
    // TestVGA();
    // TestBIOSFunctions(BootDriveNumber);
    StringTests();

    while (1) {};
}