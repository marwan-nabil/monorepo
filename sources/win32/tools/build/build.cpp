#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <shellapi.h>

#include "sources\win32\shared\base_types.h"
#include "sources\win32\shared\basic_defines.h"
#include "sources\win32\shared\math\scalar_conversions.h"
#include "sources\win32\shared\console\console.h"
#include "sources\win32\shared\file_system\files.h"
#include "sources\win32\shared\strings\strings.h"
#include "sources\win32\shared\strings\path_handling.h"
#include "sources\win32\shared\fat12\fat12.h"

#include "build.h"

#include "sources\win32\shared\console\console.cpp"
#include "sources\win32\shared\file_system\files.cpp"
#include "sources\win32\shared\file_system\folders.cpp"
#include "sources\win32\shared\strings\path_handling.cpp"
#include "sources\win32\shared\system\processes.cpp"
#include "sources\win32\shared\fat12\fat12_get.cpp"
#include "sources\win32\shared\fat12\fat12_set.cpp"
#include "sources\win32\shared\fat12\fat12_interface.cpp"

#include "build_helpers.cpp"
#include "msvc_helpers.cpp"
#include "nasm_helpers.cpp"
#include "gcc_cross_compiler_helpers.cpp"

#include "sources\win32\tools\lint\build.cpp"
#include "sources\win32\tools\fetch_data\build.cpp"
#include "sources\win32\tests\compilation_tests\build.cpp"
#include "sources\win32\tests\fat12_tests\build.cpp"
#include "sources\win32\simulator\build.cpp"
#include "sources\win32\demos\directx\build.cpp"
#include "sources\win32\handmade_hero\build.cpp"
#include "sources\win32\demos\imgui\build.cpp"
#include "sources\win32\ray_tracer\build.cpp"
#include "sources\i686-elf\boot_sector\build.cpp"
#include "sources\i686-elf\bootloader\build.cpp"
#include "sources\i686-elf\kernel\build.cpp"
#include "sources\i686-elf\floppy_image\build.cpp"

build_target_config BuildTargetConfigurations[] =
{
    {"lint", &BuildLint, "[job_per_directory]", NULL, NULL},
    {"fetch_data", &BuildFetchData, NULL, NULL, NULL},
    {"compilation_tests", &BuildCompilationTests, NULL, NULL, NULL},
    {"fat12_tests", &BuildFat12Tests, NULL, NULL, NULL},
    {"simulator", &BuildSimulator, NULL, NULL, NULL},
    {"directx_demo", &BuildDirectxDemo, "[debug, release]", NULL, NULL},
    {"handmade_hero", &BuildHandmadeHero, NULL, NULL, NULL},
    {"imgui_demo", &BuildImguiDemo, "[opengl2, dx11]", NULL, NULL},
    {"ray_tracer", &BuildRayTracer, "[1_lane, 4_lanes, 8_lanes]", NULL, NULL},
    {"os", &BuildOsFloppyDiskImage, NULL, NULL, NULL},
};

console_context GlobalConsoleContext;

static void DisplayHelp()
{
    printf("INFO: Available build targets:\n");
    printf("          build help\n");
    printf("          build clean\n");
    printf("          build clean_all\n");

    for (u32 TargetIndex = 0; TargetIndex < ArrayCount(BuildTargetConfigurations); TargetIndex++)
    {
        printf("          build %s ", BuildTargetConfigurations[TargetIndex].TargetName);
        if (BuildTargetConfigurations[TargetIndex].FirstArgument)
        {
            printf("%s ", BuildTargetConfigurations[TargetIndex].FirstArgument);
        }
        if (BuildTargetConfigurations[TargetIndex].SecondArgument)
        {
            printf("%s ", BuildTargetConfigurations[TargetIndex].SecondArgument);
        }
        if (BuildTargetConfigurations[TargetIndex].ThirdArgument)
        {
            printf("%s ", BuildTargetConfigurations[TargetIndex].ThirdArgument);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    u32 BuildSuccess = FALSE;
    build_context BuildContext = {};

    _getcwd
    (
        BuildContext.EnvironmentInfo.OutputDirectoryPath,
        sizeof(BuildContext.EnvironmentInfo.OutputDirectoryPath)
    );

    StringCchCatA
    (
        BuildContext.EnvironmentInfo.RootDirectoryPath,
        ArrayCount(BuildContext.EnvironmentInfo.RootDirectoryPath),
        BuildContext.EnvironmentInfo.OutputDirectoryPath
    );
    RemoveLastSegmentFromPath(BuildContext.EnvironmentInfo.RootDirectoryPath);

    BuildContext.EnvironmentInfo.argc = argc;
    BuildContext.EnvironmentInfo.argv = argv;
    BuildContext.EnvironmentInfo.ConsoleContext = &GlobalConsoleContext;

    InitializeConsole(&GlobalConsoleContext);

    if (argc < 2)
    {
        ConsolePrintColored("ERROR: No build target.\n", &GlobalConsoleContext, FOREGROUND_RED);
        DisplayHelp();
        return 1;
    }

    if (strcmp(argv[1], "clean") == 0)
    {
        for (u32 TargetIndex = 0; TargetIndex < ArrayCount(BuildTargetConfigurations); TargetIndex++)
        {
            if (DoesDirectoryExist(BuildTargetConfigurations[TargetIndex].TargetName))
            {
                DeleteDirectoryCompletely(BuildTargetConfigurations[TargetIndex].TargetName);
            }
        }
        BuildSuccess = TRUE;
    }
    else if (strcmp(argv[1], "clean_all") == 0)
    {
        EmptyDirectory(BuildContext.EnvironmentInfo.OutputDirectoryPath);
        BuildSuccess = TRUE;
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        DisplayHelp();
        BuildSuccess = TRUE;
    }
    else
    {
        build_target_config *FoundTargetConfig = NULL;
        for (u32 TargetIndex = 0; TargetIndex < ArrayCount(BuildTargetConfigurations); TargetIndex++)
        {
            if (strcmp(argv[1], BuildTargetConfigurations[TargetIndex].TargetName) == 0)
            {
                FoundTargetConfig = &BuildTargetConfigurations[TargetIndex];
                break;
            }
        }

        if (FoundTargetConfig)
        {
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
                FoundTargetConfig->TargetName
            );

            CreateDirectoryA(BuildContext.EnvironmentInfo.TargetOutputDirectoryPath, NULL);
            b32 Result = SetCurrentDirectory(BuildContext.EnvironmentInfo.TargetOutputDirectoryPath);
            if (Result)
            {
                BuildSuccess = FoundTargetConfig->BuildFunction(&BuildContext);
                SetCurrentDirectory(BuildContext.EnvironmentInfo.OutputDirectoryPath);
            }
        }
        else
        {
            ConsoleSwitchColor(&GlobalConsoleContext, FOREGROUND_RED);
            printf("ERROR: invalid build target \"%s\".\n", argv[1]);
            ConsoleResetColor(&GlobalConsoleContext);
            DisplayHelp();
            BuildSuccess = FALSE;
        }
    }

    if (BuildSuccess)
    {
        ConsolePrintColored("INFO: Build Succeeded.\n", &GlobalConsoleContext, FOREGROUND_GREEN);
        return 0;
    }
    else
    {
        ConsolePrintColored("ERROR: Build Failed.\n", &GlobalConsoleContext, FOREGROUND_RED);
        return 1;
    }
}