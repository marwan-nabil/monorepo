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
#include "sources\win32\shared\console\console.h"
#include "sources\win32\shared\file_system\folders.h"
#include "sources\win32\shared\strings\strings.h"
#include "sources\win32\shared\strings\path_handling.h"
#include "sources\win32\shared\system\processes.h"

#include "build.h"
#include "build_helpers.h"
#include "msvc_helpers.h"
#include "nasm_helpers.h"
#include "gcc_helpers.h"

#include "sources\win32\shared\console\console.cpp"
#include "sources\win32\shared\file_system\folders.cpp"
#include "sources\win32\shared\strings\path_handling.cpp"
#include "sources\win32\shared\system\processes.cpp"

#include "build_helpers.cpp"
#include "msvc_helpers.cpp"
#include "nasm_helpers.cpp"
#include "gcc_helpers.cpp"

build_target_config *GlobalBuildTargetConfigurations;
u32 GlobalBuildTargetCount;
build_context GlobalBuildContext;

// build_target_config BuildTargetConfigurations[] =
// {
//     {"lint", &BuildLint, "[job_per_directory]", NULL, NULL},
//     {"fetch_data", &BuildFetchData, NULL, NULL, NULL},
//     {"compilation_tests", &BuildCompilationTests, NULL, NULL, NULL},
//     {"fat12_tests", &BuildFat12Tests, NULL, NULL, NULL},
//     {"simulator", &BuildSimulator, NULL, NULL, NULL},
//     {"directx_demo", &BuildDirectxDemo, "[debug, release]", NULL, NULL},
//     {"handmade_hero", &BuildHandmadeHero, NULL, NULL, NULL},
//     {"imgui_demo", &BuildImguiDemo, "[opengl2, dx11]", NULL, NULL},
//     {"ray_tracer", &BuildRayTracer, "[1_lane, 4_lanes, 8_lanes]", NULL, NULL},
//     {"os", &BuildOsFloppyDiskImage, NULL, NULL, NULL},
// };

void PopulateBuildTargetConfigurationsList()
{

}

int main(int argc, char **argv)
{
    u32 BuildSuccess = FALSE;

    _getcwd
    (
        GlobalBuildContext.EnvironmentInfo.OutputDirectoryPath,
        sizeof(GlobalBuildContext.EnvironmentInfo.OutputDirectoryPath)
    );

    StringCchCatA
    (
        GlobalBuildContext.EnvironmentInfo.RootDirectoryPath,
        ArrayCount(GlobalBuildContext.EnvironmentInfo.RootDirectoryPath),
        GlobalBuildContext.EnvironmentInfo.OutputDirectoryPath
    );
    RemoveLastSegmentFromPath(GlobalBuildContext.EnvironmentInfo.RootDirectoryPath);

    StringCchCatA
    (
        GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath,
        ArrayCount(GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath),
        GlobalBuildContext.EnvironmentInfo.OutputDirectoryPath
    );

    GlobalBuildContext.EnvironmentInfo.argc = argc;
    GlobalBuildContext.EnvironmentInfo.argv = argv;

    InitializeConsole(&GlobalBuildContext.EnvironmentInfo.ConsoleContext);

    PopulateBuildTargetConfigurationsList();

    if (argc < 2)
    {
        ConsolePrintColored("ERROR: No build target.\n", &GlobalBuildContext.EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
        DisplayHelp();
        return 1;
    }

    if (strcmp(argv[1], "clean") == 0)
    {
        for (u32 TargetIndex = 0; TargetIndex < GlobalBuildTargetCount; TargetIndex++)
        {
            if (DoesDirectoryExist(GlobalBuildTargetConfigurations[TargetIndex].TargetName))
            {
                DeleteDirectoryCompletely(GlobalBuildTargetConfigurations[TargetIndex].TargetName);
            }
        }
        BuildSuccess = TRUE;
    }
    else if (strcmp(argv[1], "clean_all") == 0)
    {
        EmptyDirectory(GlobalBuildContext.EnvironmentInfo.OutputDirectoryPath);
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
        for (u32 TargetIndex = 0; TargetIndex < GlobalBuildTargetCount; TargetIndex++)
        {
            if (strcmp(argv[1], GlobalBuildTargetConfigurations[TargetIndex].TargetName) == 0)
            {
                FoundTargetConfig = &GlobalBuildTargetConfigurations[TargetIndex];
                break;
            }
        }

        if (FoundTargetConfig)
        {
            StringCchCatA
            (
                GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath,
                ArrayCount(GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath),
                "\\"
            );
            StringCchCatA
            (
                GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath,
                ArrayCount(GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath),
                FoundTargetConfig->TargetName
            );

            CreateDirectoryA(GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath, NULL);
            b32 Result = SetCurrentDirectory(GlobalBuildContext.EnvironmentInfo.TargetOutputDirectoryPath);
            if (Result)
            {
                BuildSuccess = FoundTargetConfig->BuildFunction(&GlobalBuildContext);
                SetCurrentDirectory(GlobalBuildContext.EnvironmentInfo.OutputDirectoryPath);
            }
        }
        else
        {
            ConsoleSwitchColor(&GlobalBuildContext.EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
            printf("ERROR: invalid build target \"%s\".\n", argv[1]);
            ConsoleResetColor(&GlobalBuildContext.EnvironmentInfo.ConsoleContext);
            DisplayHelp();
            BuildSuccess = FALSE;
        }
    }

    if (BuildSuccess)
    {
        ConsolePrintColored("INFO: Build Succeeded.\n", &GlobalBuildContext.EnvironmentInfo.ConsoleContext, FOREGROUND_GREEN);
        return 0;
    }
    else
    {
        ConsolePrintColored("ERROR: Build Failed.\n", &GlobalBuildContext.EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
        return 1;
    }
}