#include <stdarg.h>
#include "libraries\x86\base_types.h"
#include "libraries\x86\basic_defines.h"
#include "libraries\x86\vga\vga.h"
#include "libraries\x86\bios\disk.h"
#include "libraries\x86\disk\disk.h"
#include "libraries\x86\io\io.h"
#include "libraries\x86\math\integers.h"
#include "libraries\x86\strings\print.h"
#include "libraries\x86\fat12\fat12.h"
#include "libraries\x86\path_handling\path_handling.h"
#include "libraries\x86\memory\arena_allocator.h"
#include "libraries\x86\file_io\file_io.h"
#include "projects\os\bootloader\memory_layout.h"

#include "libraries\x86\vga\vga.c"
#include "libraries\x86\timing.c"
#include "libraries\x86\memory\memory.c"
#include "libraries\x86\strings\strings.c"
#include "libraries\x86\strings\print.c"
#include "libraries\x86\disk\disk.c"
#include "libraries\x86\memory\arena_allocator.c"
#include "libraries\x86\path_handling\path_handling.c"
#include "libraries\x86\fat12\get.c"
#include "libraries\x86\fat12\set.c"
#include "libraries\x86\file_io\file_io.c"
#include "projects\os\bootloader\tests.c"

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

    SpinlockSleep(800000);

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