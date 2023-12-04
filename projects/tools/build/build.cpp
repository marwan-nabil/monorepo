#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <shellapi.h>

#include "platforms\win32\base_types.h"
#include "platforms\win32\basic_defines.h"
#include "platforms\win32\console\console.h"
#include "platforms\win32\file_system\files.h"
#include "platforms\win32\strings\path_handling.h"
#include "platforms\win32\fat12\fat12.h"

#include "build.h"

#include "platforms\win32\math\scalar_conversions.cpp"
#include "platforms\win32\strings\strings.cpp"
#include "platforms\win32\console\console.cpp"
#include "platforms\win32\file_system\files.cpp"
#include "platforms\win32\file_system\folders.cpp"
#include "platforms\win32\strings\path_handling.cpp"
#include "platforms\win32\processes\processes.cpp"
#include "platforms\win32\fat12\fat12_get.cpp"
#include "platforms\win32\fat12\fat12_set.cpp"
#include "platforms\win32\fat12\fat12_interface.cpp"

#include "build_helpers.cpp"
#include "msvc_helpers.cpp"
#include "nasm_helpers.cpp"
#include "gcc_cross_compiler_helpers.cpp"

#include "projects\tools\lint\build.cpp"
#include "projects\tools\fetch_data\build.cpp"
#include "projects\tests\compilation_tests\build.cpp"
#include "projects\tests\fat12_tests\build.cpp"
#include "projects\simulator\build.cpp"
#include "projects\demos\directx\build.cpp"
#include "projects\handmade_hero\build.cpp"
#include "projects\demos\imgui\build.cpp"
#include "projects\ray_tracer\build.cpp"
#include "projects\os\boot_sector\build.cpp"
#include "projects\os\bootloader\build.cpp"
#include "projects\os\kernel\build.cpp"
#include "projects\os\build.cpp"

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
    {"os", &BuildX86RealOs, NULL, NULL, NULL},
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