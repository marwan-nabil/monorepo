#include <stdarg.h>
#include "libraries\x86\base_types.h"
#include "libraries\x86\basic_defines.h"
#include "libraries\x86\vga\vga.h"
// #include "libraries\x86\bios\disk.h"
// #include "libraries\x86\disk\disk.h"
#include "libraries\x86\io\io.h"
#include "libraries\x86\math\integers.h"
#include "libraries\x86\strings\print.h"
// #include "libraries\x86\fat12\fat12.h"
#include "libraries\x86\path_handling\path_handling.h"
#include "libraries\x86\memory\arena_allocator.h"
// #include "libraries\x86\file_io\file_io.h"

#include "libraries\x86\vga\vga.c"
#include "libraries\x86\timing.c"
#include "libraries\x86\memory\memory.c"
#include "libraries\x86\strings\strings.c"
#include "libraries\x86\strings\print.c"
// #include "libraries\x86\disk\disk.c"
#include "libraries\x86\memory\arena_allocator.c"
#include "libraries\x86\path_handling\path_handling.c"
// #include "libraries\x86\fat12\get.c"
// #include "libraries\x86\fat12\set.c"
// #include "libraries\x86\file_io\file_io.c"

extern u8 __bss_start;
extern u8 __end;

u8 FreeStore[KiloBytes(64)];
print_context GlobalPrintContext;

void __attribute__((section(".entry"))) Start()
{
    MemoryZero(&__bss_start, (&__end) - (&__bss_start));

    ClearScreen();
    PrintString(&GlobalPrintContext, "\r\nHello from the kernel!\r\n");

    while (1) {};
}