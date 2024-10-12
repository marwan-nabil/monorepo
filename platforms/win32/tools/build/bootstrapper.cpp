#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>

#include "platforms\win32\libraries\base_types.h"
#include "platforms\win32\libraries\basic_defines.h"
#include "platforms\win32\libraries\strings\strings.h"
#include "platforms\win32\libraries\strings\string_list.h"
#include "platforms\win32\libraries\shell\console.h"
#include "platforms\win32\libraries\file_system\folders.h"
#include "platforms\win32\libraries\strings\path_handling.h"
#include "platforms\win32\tools\build\actions\build_context.h"
#include "platforms\win32\tools\build\actions\msvc.h"
#include "platforms\win32\tools\build\targets.h"
#include "platforms\win32\tools\build\build.h"

b32 BuildBuild(build_context *BuildContext)
{
    b32 BuildSuccess = FALSE;

    char *StaticCompilerFlags =
        "/nologo /Oi /FC /GR- /EHa- "
        "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 "
        "/D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS ";

    char *DynamicCompilerFlags = NULL;
    if
    (
        (BuildContext->EnvironmentInfo.argc == 2) &&
        (strcmp(BuildContext->EnvironmentInfo.argv[1], "debug") == 0)
    )
    {
        DynamicCompilerFlags = "/Od /Z7 ";
    }
    else
    {
        DynamicCompilerFlags = "/O2 ";
    }

    char *SourceFiles[]
    {
        "\\platforms\\win32\\tools\\build\\actions\\build_context.cpp",
        "\\platforms\\win32\\tools\\build\\actions\\gcc.cpp",
        "\\platforms\\win32\\tools\\build\\actions\\iverilog.cpp",
        "\\platforms\\win32\\tools\\build\\actions\\msvc.cpp",
        "\\platforms\\win32\\tools\\build\\actions\\nasm.cpp",
        "\\platforms\\win32\\tools\\build\\build.cpp",
        "\\platforms\\win32\\tools\\build\\targets.cpp",
        "\\platforms\\hdl\\uart_app\\build.cpp",
        "\\platforms\\hdl\\verilog_demo\\build.cpp",
        "\\platforms\\i686-elf\\bootloader\\build.cpp",
        "\\platforms\\i686-elf\\bootsector\\build.cpp",
        "\\platforms\\i686-elf\\kernel\\build.cpp",
        "\\platforms\\i686-elf\\build.cpp",
        "\\platforms\\win32\\applications\\handmade_hero\\build.cpp",
        "\\platforms\\win32\\applications\\ray_tracer\\build.cpp",
        "\\platforms\\win32\\applications\\simulator\\build.cpp",
        "\\platforms\\win32\\demos\\directx\\build.cpp",
        "\\platforms\\win32\\demos\\imgui\\build.cpp",
        "\\platforms\\win32\\demos\\refterm\\build.cpp",
        "\\platforms\\win32\\tests\\build_tests\\build.cpp",
        "\\platforms\\win32\\tests\\fat12_tests\\build.cpp",
        "\\platforms\\win32\\tools\\fetch_data\\build.cpp",
        "\\platforms\\win32\\tools\\lint\\build.cpp",
        "\\platforms\\win32\\libraries\\shell\\console.cpp",
        "\\platforms\\win32\\libraries\\file_system\\fat12\\fat12_get.cpp",
        "\\platforms\\win32\\libraries\\file_system\\fat12\\fat12_set.cpp",
        "\\platforms\\win32\\libraries\\file_system\\fat12\\fat12_interface.cpp",
        "\\platforms\\win32\\libraries\\file_system\\files.cpp",
        "\\platforms\\win32\\libraries\\file_system\\folders.cpp",
        "\\platforms\\win32\\libraries\\strings\\path_handling.cpp",
        "\\platforms\\win32\\libraries\\strings\\string_list.cpp",
        "\\platforms\\win32\\libraries\\system\\processes.cpp",
    };

    for (u32 Index = 0; Index < ArrayCount(SourceFiles); Index++)
    {
        // TODO: optimize this loop
        AddCompilerSourceFile(BuildContext, SourceFiles[Index]);

        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        char *SourceFileNameWithExtension = GetPointerToLastSegmentFromPath(SourceFiles[Index]);
        char SourceFileName[_MAX_FNAME] = {};
        char SourceFileExtension[_MAX_EXT] = {};
        GetFileNameAndExtensionFromString(SourceFileNameWithExtension, SourceFileName, _MAX_FNAME, SourceFileExtension, _MAX_EXT);
        if (strcmp(SourceFileName, "build") == 0)
        {
            char TemporaryFilePathBuffer[MAX_PATH] = {};
            StringCbCat(TemporaryFilePathBuffer, ArrayCount(TemporaryFilePathBuffer), SourceFiles[Index]);
            RemoveLastSegmentFromPath(TemporaryFilePathBuffer, FALSE, '\\');
            char *ParentFolderName = GetPointerToLastSegmentFromPath(TemporaryFilePathBuffer);
            StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ParentFolderName);
            StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "_");
            StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), SourceFileName);
        }
        else
        {
            StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), SourceFileName);
        }
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ".obj");
        SetCompilerOutputObject(BuildContext, ObjectFileName);

        AddCompilerFlags(BuildContext, StaticCompilerFlags);
        AddCompilerFlags(BuildContext, DynamicCompilerFlags);
        SetCompilerIncludePath(BuildContext, "\\");
        BuildSuccess = CompileWithMSVC(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
        ClearBuildContext(BuildContext);
    }

    string_node *ObjectFiles = GetListOfFilesWithExtensionInFolder
    (
        BuildContext->EnvironmentInfo.TargetOutputDirectoryPath,
        "obj"
    );
    string_node *CurrentNode = ObjectFiles;
    while (CurrentNode)
    {
        char ObjectFileName[MAX_PATH] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), GetPointerToLastSegmentFromPath(CurrentNode->String));
        AddLinkerInputFile(BuildContext, ObjectFileName);
        CurrentNode = CurrentNode->NextString;
    }
    FreeStringList(ObjectFiles);

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib shell32.lib");
    SetLinkerOutputBinary(BuildContext, "\\build.exe");
    BuildSuccess = LinkWithMSVC(BuildContext);
    return BuildSuccess;
}

int main(int argc, char **argv)
{
    u32 BuildSuccess = FALSE;
    build_context BuildContext = {};

    _getcwd
    (
        BuildContext.EnvironmentInfo.RootDirectoryPath,
        sizeof(BuildContext.EnvironmentInfo.RootDirectoryPath)
    );

    StringCchCatA
    (
        BuildContext.EnvironmentInfo.OutputDirectoryPath,
        ArrayCount(BuildContext.EnvironmentInfo.OutputDirectoryPath),
        BuildContext.EnvironmentInfo.RootDirectoryPath
    );
    StringCchCatA
    (
        BuildContext.EnvironmentInfo.OutputDirectoryPath,
        ArrayCount(BuildContext.EnvironmentInfo.OutputDirectoryPath),
        "\\outputs"
    );

    BuildContext.EnvironmentInfo.argc = argc;
    BuildContext.EnvironmentInfo.argv = argv;

    InitializeConsole();

    StringCchCatA
    (
        BuildContext.EnvironmentInfo.TargetOutputDirectoryPath,
        ArrayCount(BuildContext.EnvironmentInfo.TargetOutputDirectoryPath),
        BuildContext.EnvironmentInfo.OutputDirectoryPath
    );
    StringCchCatA
    (
        BuildContext.EnvironmentInfo.TargetOutputDirectoryPath,
        ArrayCount(BuildContext.EnvironmentInfo.TargetOutputDirectoryPath),
        "\\"
    );
    StringCchCatA
    (
        BuildContext.EnvironmentInfo.TargetOutputDirectoryPath,
        ArrayCount(BuildContext.EnvironmentInfo.TargetOutputDirectoryPath),
        "build"
    );

    CreateDirectoryA(BuildContext.EnvironmentInfo.TargetOutputDirectoryPath, NULL);
    b32 Result = SetCurrentDirectory(BuildContext.EnvironmentInfo.TargetOutputDirectoryPath);
    if (Result)
    {
        BuildSuccess = BuildBuild(&BuildContext);
        SetCurrentDirectory(BuildContext.EnvironmentInfo.RootDirectoryPath);
    }

    if (BuildSuccess)
    {
        ConsolePrintColored("INFO: Build Succeeded.\n", FOREGROUND_GREEN);
        return 0;
    }
    else
    {
        ConsolePrintColored("ERROR: Build Failed.\n", FOREGROUND_RED);
        return 1;
    }
}