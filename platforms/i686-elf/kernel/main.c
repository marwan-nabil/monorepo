#include <stdarg.h>
#include "platforms\i686-elf\libraries\base_types.h"
#include "platforms\i686-elf\libraries\basic_defines.h"
#include "platforms\i686-elf\libraries\vga\vga.h"
#include "platforms\i686-elf\libraries\memory\memory.h"
#include "platforms\i686-elf\libraries\strings\print.h"
#include "platforms\i686-elf\libraries\cpu\gdt.h"
#include "platforms\i686-elf\libraries\cpu\idt.h"
#include "platforms\i686-elf\kernel\linker.h"
#include "platforms\i686-elf\kernel\tests.h"
#include "platforms\i686-elf\kernel\descriptor_tables.h"

u8 FreeStore[KiloBytes(64)];
print_context GlobalPrintContext;

void __attribute__((section(".entry"))) Start()
{
    MemoryZero(&__bss_start, &__bss_end - &__bss_start);

    InitializeRamGDT();
    GlobalGDTDescriptor.Limit = sizeof(GlobalRamGDT) - 1;
    GlobalGDTDescriptor.Entry = (gdt_entry *)GlobalRamGDT;
    LoadGDT(&GlobalGDTDescriptor, 0x08, 0x10);

    InitializeRamIDT();
    EnableAllISRs();
    GlobalIDTDescriptor.Limit = sizeof(GlobalRamIDT) - 1;
    GlobalIDTDescriptor.FirstEntry = (idt_entry *)GlobalRamIDT;
    LoadIDT(&GlobalIDTDescriptor);

    ClearScreen();
    PrintString(&GlobalPrintContext, "\nHello from the kernel!\n\n");
    TestInterrupts();

    while (1) {};
}