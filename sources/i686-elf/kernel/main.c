#include <stdarg.h>
#include "sources\i686-elf\base_types.h"
#include "sources\i686-elf\basic_defines.h"
#include "sources\i686-elf\vga\vga.h"
#include "sources\i686-elf\memory\memory.h"
#include "sources\i686-elf\strings\print.h"
#include "sources\i686-elf\cpu\gdt.h"
#include "sources\i686-elf\cpu\idt.h"
#include "sources\i686-elf\kernel\linker.h"
#include "sources\i686-elf\kernel\tests.h"
#include "sources\i686-elf\kernel\descriptor_tables.h"

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