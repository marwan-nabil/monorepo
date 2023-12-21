#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\console\console.h"
#include "sources\win32\strings\strings.h"
#include "sources\win32\strings\path_handling.h"
#include "sources\win32\fat12\fat12.h"
#include "sources\win32\fat12\fat12_interface.h"
#include "sources\win32\file_system\files.h"
#include "build.h"
#include "build_helpers.h"
#include "compiler_helpers.h"

b32 BuildBootSectorImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootsector");
    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\bootsector\\entry.s");
    AddCompilerFlags(BuildContext, "-f bin -lboot_sector.lst");
    SetCompilerIncludePath(BuildContext, "\\");
    SetCompilerOutputObject(BuildContext, "\\bootsector.img");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    PopSubTarget(BuildContext);
    return BuildSuccess;
}

b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");
    b32 BuildSuccess = FALSE;

    char *AssemblyFiles[] =
    {
        "\\sources\\i686-elf\\bootloader\\entry.s",
        "\\sources\\i686-elf\\cpu\\io.s",
        "\\sources\\i686-elf\\bios\\disk.s"
    };

    for (u32 Index = 0; Index < ArrayCount(AssemblyFiles); Index++)
    {
        AddCompilerSourceFile(BuildContext, AssemblyFiles[Index]);

        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(AssemblyFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        SetCompilerOutputObject(BuildContext, ObjectFileName);

        AddCompilerFlags(BuildContext, "-f elf");
        SetCompilerIncludePath(BuildContext, "\\");
        BuildSuccess = AssembleWithNasm(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
    }

    char *CFiles[] =
    {
        "\\sources\\i686-elf\\bootloader\\main.c",
        "\\sources\\i686-elf\\bootloader\\tests.c",
        "\\sources\\i686-elf\\vga\\vga.c",
        "\\sources\\i686-elf\\disk\\disk.c",
        "\\sources\\i686-elf\\strings\\strings.c",
        "\\sources\\i686-elf\\strings\\path_handling.c",
        "\\sources\\i686-elf\\strings\\print.c",
        "\\sources\\i686-elf\\cpu\\timing.c",
        "\\sources\\i686-elf\\fat12\\get.c",
        "\\sources\\i686-elf\\fat12\\set.c",
        "\\sources\\i686-elf\\memory\\arena_allocator.c",
        "\\sources\\i686-elf\\memory\\memory.c",
        "\\sources\\i686-elf\\file_io\\file_io.c",
    };

    for (u32 Index = 0; Index < ArrayCount(CFiles); Index++)
    {
        AddCompilerSourceFile(BuildContext, CFiles[Index]);

        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(CFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        SetCompilerOutputObject(BuildContext, ObjectFileName);

        AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
        SetCompilerIncludePath(BuildContext, "\\");
        BuildSuccess = CompileWithGCC(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
    }

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=bootloader.map");
    SetLinkerScriptPath(BuildContext, "\\sources\\i686-elf\\bootloader\\linker.lds");

    for (u32 Index = 0; Index < ArrayCount(AssemblyFiles); Index++)
    {
        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(AssemblyFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }
    for (u32 Index = 0; Index < ArrayCount(CFiles); Index++)
    {
        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(CFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }
    
    SetLinkerOutputBinary(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}


b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");
    b32 BuildSuccess = FALSE;

    char *AssemblyFiles[] =
    {
        "\\sources\\i686-elf\\kernel\\isr.s",
        "\\sources\\i686-elf\\cpu\\gdt.s",
        "\\sources\\i686-elf\\cpu\\idt.s",
        "\\sources\\i686-elf\\cpu\\io.s",
        "\\sources\\i686-elf\\cpu\\panic.s",
    };

    for (u32 Index = 0; Index < ArrayCount(AssemblyFiles); Index++)
    {
        AddCompilerSourceFile(BuildContext, AssemblyFiles[Index]);

        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(AssemblyFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        SetCompilerOutputObject(BuildContext, ObjectFileName);

        AddCompilerFlags(BuildContext, "-f elf");
        SetCompilerIncludePath(BuildContext, "\\");
        BuildSuccess = AssembleWithNasm(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
    }

    char *CFiles[] =
    {
        "\\sources\\i686-elf\\kernel\\main.c",
        "\\sources\\i686-elf\\kernel\\isr.c",
        "\\sources\\i686-elf\\kernel\\descriptor_tables.c",
        "\\sources\\i686-elf\\kernel\\tests.c",
        "\\sources\\i686-elf\\vga\\vga.c",
        "\\sources\\i686-elf\\strings\\strings.c",
        "\\sources\\i686-elf\\strings\\path_handling.c",
        "\\sources\\i686-elf\\strings\\print.c",
        "\\sources\\i686-elf\\cpu\\timing.c",
        "\\sources\\i686-elf\\cpu\\gdt.c",
        "\\sources\\i686-elf\\cpu\\idt.c",
        "\\sources\\i686-elf\\memory\\arena_allocator.c",
        "\\sources\\i686-elf\\memory\\memory.c",
    };

    for (u32 Index = 0; Index < ArrayCount(CFiles); Index++)
    {
        AddCompilerSourceFile(BuildContext, CFiles[Index]);

        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(CFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        SetCompilerOutputObject(BuildContext, ObjectFileName);

        AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
        SetCompilerIncludePath(BuildContext, "\\");
        BuildSuccess = CompileWithGCC(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
    }

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=kernel.map");
    SetLinkerScriptPath(BuildContext, "\\sources\\i686-elf\\kernel\\linker.lds");
    
    for (u32 Index = 0; Index < ArrayCount(AssemblyFiles); Index++)
    {
        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(AssemblyFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }
    for (u32 Index = 0; Index < ArrayCount(CFiles); Index++)
    {
        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, MAX_PATH, "\\");
        StringCchCat(ObjectFileName, MAX_PATH, GetPointerToLastSegmentFromPath(CFiles[Index]));
        StringCchCat(ObjectFileName, MAX_PATH, ".elf");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }

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
    StringCchCat(ImagePath, ArrayCount(ImagePath), "\\bootsector\\bootsector.img");

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