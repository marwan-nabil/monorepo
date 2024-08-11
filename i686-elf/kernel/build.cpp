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

b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");
    b32 BuildSuccess = FALSE;

    char *AssemblyFiles[] =
    {
        "\\i686-elf\\kernel\\isr.s",
        "\\i686-elf\\libraries\\cpu\\gdt.s",
        "\\i686-elf\\libraries\\cpu\\idt.s",
        "\\i686-elf\\libraries\\cpu\\io.s",
        "\\i686-elf\\libraries\\cpu\\panic.s",
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
        "\\i686-elf\\kernel\\main.c",
        "\\i686-elf\\kernel\\isr.c",
        "\\i686-elf\\kernel\\descriptor_tables.c",
        "\\i686-elf\\kernel\\tests.c",
        "\\i686-elf\\libraries\\vga\\vga.c",
        "\\i686-elf\\libraries\\strings\\strings.c",
        "\\i686-elf\\libraries\\strings\\path_handling.c",
        "\\i686-elf\\libraries\\strings\\print.c",
        "\\i686-elf\\libraries\\cpu\\timing.c",
        "\\i686-elf\\libraries\\cpu\\gdt.c",
        "\\i686-elf\\libraries\\cpu\\idt.c",
        "\\i686-elf\\libraries\\memory\\arena_allocator.c",
        "\\i686-elf\\libraries\\memory\\memory.c",
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
    SetLinkerScriptPath(BuildContext, "\\i686-elf\\kernel\\kernel.lds");
    
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