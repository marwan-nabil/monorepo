#include <stdarg.h>
#include "sources\i686-elf\shared\base_types.h"
#include "sources\i686-elf\shared\basic_defines.h"
#include "sources\i686-elf\shared\vga\vga.h"
#include "sources\i686-elf\shared\bios\disk.h"
#include "sources\i686-elf\shared\disk\disk.h"
#include "sources\i686-elf\shared\cpu\io.h"
#include "sources\i686-elf\shared\math\integers.h"
#include "sources\i686-elf\shared\strings\print.h"
#include "sources\i686-elf\shared\cpu\timing.h"
#include "sources\i686-elf\shared\fat12\fat12.h"
#include "sources\i686-elf\shared\memory\arena_allocator.h"
#include "sources\i686-elf\shared\strings\path_handling.h"
#include "sources\i686-elf\shared\memory\arena_allocator.h"
#include "sources\i686-elf\shared\file_io\file_io.h"
#include "sources\i686-elf\bootloader\memory_layout.h"

#include "sources\i686-elf\shared\vga\vga.c"
#include "sources\i686-elf\shared\cpu\timing.c"
#include "sources\i686-elf\shared\memory\memory.c"
#include "sources\i686-elf\shared\strings\strings.c"
#include "sources\i686-elf\shared\strings\print.c"
#include "sources\i686-elf\shared\disk\disk.c"
#include "sources\i686-elf\shared\memory\arena_allocator.c"
#include "sources\i686-elf\shared\strings\path_handling.c"
#include "sources\i686-elf\shared\fat12\get.c"
#include "sources\i686-elf\shared\fat12\set.c"
#include "sources\i686-elf\shared\file_io\file_io.c"
#include "sources\i686-elf\bootloader\tests.c"

u8 FreeStore[KiloBytes(64)];
print_context GlobalPrintContext;
u8 *KernelLoadBuffer = MEMORY_LAYOUT_KERNEL_LOAD_ADDRESS;

void LoadKernel(u32 BootDriveNumber, void *FreeMemoryArea, print_context *PrintContext)
{
    ClearScreen();
    SetCursorPosition(PrintContext, 0, 0);
    PrintString(PrintContext, "\r\n============ loading the kernel ============== \r\n");

    disk_parameters DiskParameters;
    GetDiskDriveParameters(&DiskParameters, BootDriveNumber);

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        &LocalMemoryArena,
        KiloBytes(10),
        FreeMemoryArea
    );

    file_io_context *FileIoContext = PushStruct(&LocalMemoryArena, file_io_context);
    FileIoInitialize
    (
        BootDriveNumber,
        PushArray(&LocalMemoryArena, KiloBytes(16), u8),
        KiloBytes(16),
        PrintContext,
        FileIoContext
    );

    ListDirectory(FileIoContext, "\\");

    i16 FileHandle = FileOpen(FileIoContext, "\\kernel  .bin");
    FileRead(FileIoContext, FileHandle, MEMORY_LAYOUT_KERNEL_LOAD_SIZE, KernelLoadBuffer);
    FileClose(FileIoContext, FileHandle);

    SpinlockWait(400);

    typedef void (*kernel_start_function)();
    kernel_start_function KernelStart = (kernel_start_function)KernelLoadBuffer;
    KernelStart();
}

void __attribute__((cdecl)) cstart(u32 BootDriveNumber)
{
    // TestVGA(&GlobalPrintContext);
    // TestIO(&GlobalPrintContext);
    // StringTests(&GlobalPrintContext);
    // DiskDriverTests(BootDriveNumber, FreeStore, &GlobalPrintContext);
    // AllocatorTests(FreeStore, &GlobalPrintContext);
    // PathHandlingTests(FreeStore, &GlobalPrintContext);
    // Fat12Tests(BootDriveNumber, FreeStore, &GlobalPrintContext);
    // FileIoTests(BootDriveNumber, FreeStore, &GlobalPrintContext);
    LoadKernel(BootDriveNumber, &FreeStore, &GlobalPrintContext);

    while (1) {};
}