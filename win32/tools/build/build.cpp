#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <shellapi.h>

#include "win32\shared\base_types.h"
#include "win32\shared\basic_defines.h"
#include "win32\shared\console\console.h"
#include "win32\shared\file_system\files.h"
#include "win32\shared\file_system\path_handling.h"
#include "win32\shared\fat12\fat12.h"

#include "build.h"

#include "win32\shared\math\scalar_conversions.cpp"
#include "win32\shared\strings\strings.cpp"
#include "win32\shared\console\console.cpp"
#include "win32\shared\file_system\files.cpp"
#include "win32\shared\file_system\folders.cpp"
#include "win32\shared\file_system\path_handling.cpp"
#include "win32\shared\processes\processes.cpp"
#include "win32\shared\fat12\fat12_get.cpp"
#include "win32\shared\fat12\fat12_set.cpp"
#include "win32\shared\fat12\fat12_interface.cpp"

#include "build_helpers.cpp"
#include "msvc_helpers.cpp"
#include "nasm_helpers.cpp"
#include "watcom_helpers.cpp"

#include "win32\tools\lint\build.cpp"
#include "win32\tools\fetch_data\build.cpp"
#include "win32\tests\compilation_tests\build.cpp"
#include "win32\tests\fat12_tests\build.cpp"
#include "win32\simulator\build.cpp"
#include "win32\demos\directx\build.cpp"
#include "win32\handmade_hero\build.cpp"
#include "win32\demos\imgui\build.cpp"
#include "win32\ray_tracer\build.cpp"
#include "x86_bios\os\boot_sector\build.cpp"
#include "x86_bios\os\bootloader\build.cpp"
#include "x86_bios\os\kernel\build.cpp"
#include "x86_bios\os\build.cpp"
#include "x86_bios\protected_mode_os\boot_sector\build.cpp"
#include "x86_bios\protected_mode_os\build.cpp"

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
    {"x86_os", &Buildx86Os, NULL, NULL, NULL},
    {"x86_protected_os", &BuildProtectedModeOs, NULL, NULL, NULL},
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

    _getcwd(BuildContext.OutputDirectoryPath, sizeof(BuildContext.OutputDirectoryPath));

    StringCchCatA
    (
        BuildContext.RootDirectoryPath,
        ArrayCount(BuildContext.RootDirectoryPath),
        BuildContext.OutputDirectoryPath
    );
    StringCchCatA
    (
        BuildContext.RootDirectoryPath,
        ArrayCount(BuildContext.RootDirectoryPath),
        "\\.."
    );

    BuildContext.argc = argc;
    BuildContext.argv = argv;
    BuildContext.ConsoleContext = &GlobalConsoleContext;

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
        EmptyDirectory(BuildContext.OutputDirectoryPath);
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
                BuildContext.TargetOutputDirectoryPath,
                ArrayCount(BuildContext.TargetOutputDirectoryPath),
                BuildContext.OutputDirectoryPath
            );
            StringCchCatA
            (
                BuildContext.TargetOutputDirectoryPath,
                ArrayCount(BuildContext.TargetOutputDirectoryPath),
                "\\"
            );
            StringCchCatA
            (
                BuildContext.TargetOutputDirectoryPath,
                ArrayCount(BuildContext.TargetOutputDirectoryPath),
                FoundTargetConfig->TargetName
            );

            CreateDirectoryA(BuildContext.TargetOutputDirectoryPath, NULL);
            b32 Result = SetCurrentDirectory(BuildContext.TargetOutputDirectoryPath);
            if (Result)
            {
                BuildSuccess = FoundTargetConfig->BuildFunction(&BuildContext);
                SetCurrentDirectory(BuildContext.OutputDirectoryPath);
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