#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\strings\path_handling.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

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
        "\\sources\\i686-elf\\storage\\disk\\disk.c",
        "\\sources\\i686-elf\\strings\\strings.c",
        "\\sources\\i686-elf\\strings\\path_handling.c",
        "\\sources\\i686-elf\\strings\\print.c",
        "\\sources\\i686-elf\\cpu\\timing.c",
        "\\sources\\i686-elf\\storage\\fat12\\get.c",
        "\\sources\\i686-elf\\storage\\fat12\\set.c",
        "\\sources\\i686-elf\\memory\\arena_allocator.c",
        "\\sources\\i686-elf\\memory\\memory.c",
        "\\sources\\i686-elf\\storage\\file_io\\file_io.c",
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