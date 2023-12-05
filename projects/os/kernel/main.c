#include <stdarg.h>
#include "libraries\i686-elf\base_types.h"
#include "libraries\i686-elf\basic_defines.h"
#include "libraries\i686-elf\vga\vga.h"
#include "libraries\i686-elf\memory\memory.h"
#include "libraries\i686-elf\strings\print.h"
#include "libraries\i686-elf\cpu\gdt.h"
#include "libraries\i686-elf\cpu\idt.h"

#include "projects\os\kernel\linker.h"
#include "projects\os\kernel\tests.h"
#include "projects\os\kernel\descriptor_tables.h"

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