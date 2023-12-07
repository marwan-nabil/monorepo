#include <stdarg.h>
#include "shared\base_types.h"
#include "shared\basic_defines.h"
#include "shared\vga\vga.h"
#include "shared\bios\disk.h"
#include "shared\disk\disk.h"
#include "shared\cpu\io.h"
#include "shared\math\integers.h"
#include "shared\strings\print.h"
#include "shared\cpu\timing.h"
#include "shared\fat12\fat12.h"
#include "shared\memory\arena_allocator.h"
#include "shared\strings\path_handling.h"
#include "shared\memory\arena_allocator.h"
#include "shared\file_io\file_io.h"
#include "bootloader\memory_layout.h"

#include "shared\vga\vga.c"
#include "shared\cpu\timing.c"
#include "shared\memory\memory.c"
#include "shared\strings\strings.c"
#include "shared\strings\print.c"
#include "shared\disk\disk.c"
#include "shared\memory\arena_allocator.c"
#include "shared\strings\path_handling.c"
#include "shared\fat12\get.c"
#include "shared\fat12\set.c"
#include "shared\file_io\file_io.c"
#include "bootloader\tests.c"

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