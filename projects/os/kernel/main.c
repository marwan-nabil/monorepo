#include <stdarg.h>
#include "platforms\i686-elf\base_types.h"
#include "platforms\i686-elf\basic_defines.h"
#include "platforms\i686-elf\vga\vga.h"
// #include "platforms\i686-elf\bios\disk.h"
// #include "platforms\i686-elf\disk\disk.h"
#include "platforms\i686-elf\io\io.h"
#include "platforms\i686-elf\math\integers.h"
#include "platforms\i686-elf\strings\print.h"
// #include "platforms\i686-elf\fat12\fat12.h"
#include "platforms\i686-elf\path_handling\path_handling.h"
#include "platforms\i686-elf\memory\arena_allocator.h"
// #include "platforms\i686-elf\file_io\file_io.h"

#include "platforms\i686-elf\vga\vga.c"
#include "platforms\i686-elf\timing.c"
#include "platforms\i686-elf\memory\memory.c"
#include "platforms\i686-elf\strings\strings.c"
#include "platforms\i686-elf\strings\print.c"
// #include "platforms\i686-elf\disk\disk.c"
#include "platforms\i686-elf\memory\arena_allocator.c"
#include "platforms\i686-elf\path_handling\path_handling.c"
// #include "platforms\i686-elf\fat12\get.c"
// #include "platforms\i686-elf\fat12\set.c"
// #include "platforms\i686-elf\file_io\file_io.c"

extern u32 __bss_start;
extern u32 __bss_end;

u8 FreeStore[KiloBytes(64)];
print_context GlobalPrintContext;

void __attribute__((section(".entry"))) Start()
{
    MemoryZero(&__bss_start, &__bss_end - &__bss_start);

    ClearScreen();
    PrintString(&GlobalPrintContext, "\r\nHello from the kernel!\r\n");

    while (1) {};
}