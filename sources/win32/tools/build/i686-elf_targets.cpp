#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\shared\base_types.h"
#include "sources\win32\shared\basic_defines.h"
#include "sources\win32\shared\console\console.h"
#include "sources\win32\shared\strings\strings.h"
#include "sources\win32\shared\fat12\fat12.h"
#include "sources\win32\shared\fat12\fat12_interface.h"
#include "sources\win32\shared\file_system\files.h"
#include "build.h"
#include "build_helpers.h"
#include "compiler_helpers.h"

b32 BuildBootSectorImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "boot_sector");
    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\boot_sector\\entry.s");
    AddCompilerFlags(BuildContext, "-f bin -lboot_sector.lst");
    SetCompilerIncludePath(BuildContext, "\\");
    SetCompilerOutputObject(BuildContext, "\\boot_sector.img");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    PopSubTarget(BuildContext);
    return BuildSuccess;
}

b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\bootloader\\entry.s");
    SetCompilerOutputObject(BuildContext, "\\entry.elf");
    AddCompilerFlags(BuildContext, "-f elf");
    SetCompilerIncludePath(BuildContext, "\\");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\bootloader\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=bootloader.map");
    SetLinkerScriptPath(BuildContext, "\\sources\\i686-elf\\bootloader\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\entry.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetLinkerOutputBinary(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}


b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");

    char *SharedCCFlags = "-std=c99 -g -ffreestanding -nostdlib";
    char *SharedAsmFlags = "-f elf";

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\platform.s");
    AddCompilerFlags(BuildContext, SharedAsmFlags);
    SetCompilerOutputObject(BuildContext, "\\platform_s.elf");
    SetCompilerIncludePath(BuildContext, "\\");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\isr.s");
    AddCompilerFlags(BuildContext, SharedAsmFlags);
    SetCompilerOutputObject(BuildContext, "\\isr_s.elf");
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\platform.c");
    SetCompilerOutputObject(BuildContext, "\\platform_c.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\isr.c");
    SetCompilerOutputObject(BuildContext, "\\isr_c.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\descriptor_tables.c");
    SetCompilerOutputObject(BuildContext, "\\descriptor_tables.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\tests.c");
    SetCompilerOutputObject(BuildContext, "\\tests.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=kernel.map");
    SetLinkerScriptPath(BuildContext, "\\sources\\i686-elf\\kernel\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\platform_s.elf");
    AddLinkerInputFile(BuildContext, "\\platform_c.elf");
    AddLinkerInputFile(BuildContext, "\\isr_s.elf");
    AddLinkerInputFile(BuildContext, "\\isr_c.elf");
    AddLinkerInputFile(BuildContext, "\\descriptor_tables.elf");
    AddLinkerInputFile(BuildContext, "\\tests.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetLinkerOutputBinary(BuildContext, "\\kernel.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}

static void AddTestFilesToDiskImage(build_context *BuildContext, fat12_disk *Fat12Disk)
{
    char TestFilePath[1024];
    ZeroMemory(TestFilePath, ArrayCount(TestFilePath));

    StringCchCat(TestFilePath, ArrayCount(TestFilePath), BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    StringCchCat(TestFilePath, ArrayCount(TestFilePath), "\\bootloader\\bootloader.img");

    read_file_result DummyBigFile = ReadFileIntoMemory(TestFilePath);

    Fat12AddDirectory(Fat12Disk, "\\Dir0");
    Fat12AddFile(Fat12Disk, "\\Dir0\\BigFile", DummyBigFile.FileMemory, DummyBigFile.Size);
    FreeFileMemory(DummyBigFile);

    char *LocalTextBuffer = "lorem epsum dolor set amet";
    Fat12AddDirectory(Fat12Disk, "\\Dir0\\Dir1");
    Fat12AddFile(Fat12Disk, "\\Dir0\\Dir1\\sample.txt", LocalTextBuffer, StringLength(LocalTextBuffer));
}

b32 BuildOsFloppyDiskImage(build_context *BuildContext)
{
    b32 BuildSuccess = BuildBootSectorImage(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    BuildSuccess = BuildBootloaderImage(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    BuildSuccess = BuildKernelImage(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    fat12_disk *Fat12Disk = (fat12_disk *)VirtualAlloc
    (
        0, sizeof(fat12_disk), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
    );
    memset(Fat12Disk, 0, sizeof(fat12_disk));

    char ImagePath[1024];
    ZeroMemory(ImagePath, ArrayCount(ImagePath));
    StringCchCat(ImagePath, ArrayCount(ImagePath), BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    StringCchCat(ImagePath, ArrayCount(ImagePath), "\\boot_sector\\boot_sector.img");

    read_file_result BootSector = ReadFileIntoMemory(ImagePath);
    memcpy(&Fat12Disk->BootSector, BootSector.FileMemory, FAT12_SECTOR_SIZE);
    FreeFileMemory(BootSector);

    ZeroMemory(ImagePath, ArrayCount(ImagePath));
    StringCchCat(ImagePath, ArrayCount(ImagePath), BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    StringCchCat(ImagePath, ArrayCount(ImagePath), "\\bootloader\\bootloader.img");

    read_file_result Bootloader = ReadFileIntoMemory(ImagePath);
    Fat12AddFile(Fat12Disk, "\\bootld  .bin", Bootloader.FileMemory, Bootloader.Size);
    FreeFileMemory(Bootloader);

    ZeroMemory(ImagePath, ArrayCount(ImagePath));
    StringCchCat(ImagePath, ArrayCount(ImagePath), BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    StringCchCat(ImagePath, ArrayCount(ImagePath), "\\kernel\\kernel.img");

    read_file_result Kernel = ReadFileIntoMemory(ImagePath);
    Fat12AddFile(Fat12Disk, "\\kernel  .bin", Kernel.FileMemory, Kernel.Size);
    FreeFileMemory(Kernel);

    // NOTE: files and direcotries added to the disk image only for testing purposes,
    //       should be removed later.
    AddTestFilesToDiskImage(BuildContext, Fat12Disk);

    ZeroMemory(ImagePath, ArrayCount(ImagePath));
    StringCchCat(ImagePath, ArrayCount(ImagePath), BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    StringCchCat(ImagePath, ArrayCount(ImagePath), "\\floppy.img");
    BuildSuccess = WriteFileFromMemory
    (
        ImagePath,
        Fat12Disk,
        sizeof(fat12_disk)
    );

    VirtualFree(Fat12Disk, 0, MEM_RELEASE);
    return BuildSuccess;
}