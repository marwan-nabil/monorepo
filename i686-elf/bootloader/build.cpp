#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "win32\libraries\base_types.h"
#include "win32\libraries\basic_defines.h"
#include "win32\libraries\strings\path_handling.h"
#include "win32\libraries\strings\string_list.h"

#include "win32\tools\build\actions\build_context.h"
#include "win32\tools\build\actions\nasm.h"
#include "win32\tools\build\actions\gcc.h"

b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");
    b32 BuildSuccess = FALSE;

    char *AssemblyFiles[] =
    {
        "\\sources\\i686-elf\\bootloader\\entry.s",
        "\\sources\\i686-elf\\libraries\\cpu\\io.s",
        "\\sources\\i686-elf\\libraries\\bios\\disk.s"
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
        "\\sources\\i686-elf\\libraries\\vga\\vga.c",
        "\\sources\\i686-elf\\libraries\\storage\\disk\\disk.c",
        "\\sources\\i686-elf\\libraries\\strings\\strings.c",
        "\\sources\\i686-elf\\libraries\\strings\\path_handling.c",
        "\\sources\\i686-elf\\libraries\\strings\\print.c",
        "\\sources\\i686-elf\\libraries\\cpu\\timing.c",
        "\\sources\\i686-elf\\libraries\\storage\\fat12\\get.c",
        "\\sources\\i686-elf\\libraries\\storage\\fat12\\set.c",
        "\\sources\\i686-elf\\libraries\\memory\\arena_allocator.c",
        "\\sources\\i686-elf\\libraries\\memory\\memory.c",
        "\\sources\\i686-elf\\libraries\\storage\\file_io\\file_io.c",
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
    SetLinkerScriptPath(BuildContext, "\\configuration\\i686-elf\\linker\\bootloader.lds");

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