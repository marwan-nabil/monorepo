// TODO: add support for disk/fat12/file_io drivers in the kernel
#include <stdarg.h>
#include "platforms\i686-elf\base_types.h"
#include "platforms\i686-elf\basic_defines.h"
#include "platforms\i686-elf\vga\vga.h"
// #include "platforms\i686-elf\bios\disk.h"
// #include "platforms\i686-elf\disk\disk.h"
#include "platforms\i686-elf\cpu\io.h"
#include "platforms\i686-elf\cpu\timing.h"
#include "platforms\i686-elf\math\integers.h"
#include "platforms\i686-elf\strings\print.h"
// #include "platforms\i686-elf\fat12\fat12.h"
#include "platforms\i686-elf\memory\arena_allocator.h"
#include "platforms\i686-elf\strings\path_handling.h"
#include "platforms\i686-elf\memory\arena_allocator.h"
// #include "platforms\i686-elf\file_io\file_io.h"
#include "platforms\i686-elf\cpu\gdt.h"
#include "platforms\i686-elf\cpu\idt.h"

#include "platforms\i686-elf\vga\vga.c"
#include "platforms\i686-elf\cpu\timing.c"
#include "platforms\i686-elf\memory\memory.c"
#include "platforms\i686-elf\strings\strings.c"
#include "platforms\i686-elf\strings\print.c"
// #include "platforms\i686-elf\disk\disk.c"
#include "platforms\i686-elf\memory\arena_allocator.c"
#include "platforms\i686-elf\strings\path_handling.c"
#include "platforms\i686-elf\cpu\gdt.c"
#include "platforms\i686-elf\cpu\idt.c"
// #include "platforms\i686-elf\fat12\get.c"
// #include "platforms\i686-elf\fat12\set.c"
// #include "platforms\i686-elf\file_io\file_io.c"

extern u32 __bss_start;
extern u32 __bss_end;

u8 FreeStore[KiloBytes(64)];
print_context GlobalPrintContext;

gdt_entry GlobalRamGDT[3];
gdt_descriptor GlobalGDTDescriptor;

idt_entry GlobalRamIDT[256];
idt_descriptor GlobalIDTDescriptor;

void InitializeRamGDT()
{
    // NULL entry
    GlobalRamGDT[0] = CreateGDTEntry(0, 0, 0, 0);

    // 32-bit code segment
    GlobalRamGDT[1] = CreateGDTEntry
    (
        0,
        0xFFFFFFFF,
        // ACCESS = 1 0 0 1  1 0 1 0
        (0 << GDTAFO_ACCESSED_BIT) |
        (1 << GDTAFO_READ_WRITE_BIT) |
        (0 << GDTAFO_DIRECTION_CONFORMING_BIT) |
        (1 << GDTAFO_EXECUTABLE_BIT) |
        (1 << GDTAFO_TYPE_BIT) |
        (0 << GDTAFO_DESCRIPTOR_PRIVILIGE_LEVEL_BIT0) |
        (0 << GDTAFO_DESCRIPTOR_PRIVILIGE_LEVEL_BIT1) |
        (1 << GDTAFO_PRESENT_BIT),
        // FLAGS = 1 1 0 0
        (0 << GDTFFO_RESERVED_BIT) |
        (0 << GDTFFO_LONG_MODE_BIT) |
        (1 << GDTFFO_DB_BIT) |
        (1 << GDTFFO_GRANULARITY_BIT)
    );

    // 32-bit data segment
    GlobalRamGDT[2] = CreateGDTEntry
    (
        0,
        0xFFFFFFFF,
        // ACCESS = 1 0 0 1  0 0 1 0
        (0 << GDTAFO_ACCESSED_BIT) |
        (1 << GDTAFO_READ_WRITE_BIT) |
        (0 << GDTAFO_DIRECTION_CONFORMING_BIT) |
        (0 << GDTAFO_EXECUTABLE_BIT) |
        (1 << GDTAFO_TYPE_BIT) |
        (0 << GDTAFO_DESCRIPTOR_PRIVILIGE_LEVEL_BIT0) |
        (0 << GDTAFO_DESCRIPTOR_PRIVILIGE_LEVEL_BIT1) |
        (1 << GDTAFO_PRESENT_BIT),
        // FLAGS = 1 1 0 0
        (0 << GDTFFO_RESERVED_BIT) |
        (0 << GDTFFO_LONG_MODE_BIT) |
        (1 << GDTFFO_DB_BIT) |
        (1 << GDTFFO_GRANULARITY_BIT)
    );
}

void InitializeRamIDT()
{
    // NULL entry
    GlobalRamGDT[0] = CreateGDTEntry(0, 0, 0, 0);
}

void __attribute__((section(".entry"))) Start()
{
    MemoryZero(&__bss_start, &__bss_end - &__bss_start);

    InitializeRamGDT();
    GlobalGDTDescriptor.Limit = sizeof(GlobalRamGDT) - 1;
    GlobalGDTDescriptor.Entry = (gdt_entry *)GlobalRamGDT;
    LoadGDT(&GlobalGDTDescriptor, 0x08, 0x10);

    GlobalIDTDescriptor.Limit = sizeof(GlobalRamIDT) - 1;
    GlobalIDTDescriptor.FirstEntry = (idt_entry *)GlobalRamIDT;
    LoadIDT(&GlobalIDTDescriptor);

    ClearScreen();
    PrintString(&GlobalPrintContext, "\r\nHello from the kernel!\r\n");

    while (1) {};
}