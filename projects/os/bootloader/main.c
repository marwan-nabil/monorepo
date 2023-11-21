#include "libraries\x86\base_types.h"
#include "libraries\x86\basic_defines.h"
#include "libraries\x86\vga\vga.h"

#include "libraries\x86\vga\vga.c"
#include "projects\os\bootloader\tests.c"

void __attribute__((cdecl)) cstart(u32 BootDriveNumber)
{
    TestVGA();

    while (1) {};
}