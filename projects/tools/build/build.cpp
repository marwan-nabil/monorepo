// TODO: each target is built in it's own directory in output/
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
#include "platforms\shared\basic_defines.h"
#include "platforms\win32\console\console.h"
#include "platforms\win32\file_system\files.h"
#include "platforms\win32\file_system\path_handling.h"
#include "platforms\win32\fat12\fat12.h"

#include "build.h"

#include "platforms\win32\math\scalar_conversions.cpp"
#include "platforms\win32\strings\strings.cpp"
#include "platforms\win32\console\console.cpp"
#include "platforms\win32\file_system\files.cpp"
#include "platforms\win32\file_system\folders.cpp"
#include "platforms\win32\file_system\path_handling.cpp"
#include "platforms\win32\processes\processes.cpp"
#include "platforms\win32\fat12\fat12_get.cpp"
#include "platforms\win32\fat12\fat12_set.cpp"
#include "platforms\win32\fat12\fat12_interface.cpp"

#include "build_helpers.cpp"
#include "msvc_helpers.cpp"
#include "nasm_helpers.cpp"
#include "watcom_helpers.cpp"

#include "projects\tools\lint\build.cpp"
#include "projects\tools\fetch_data\build.cpp"
#include "projects\tools\compilation_tests\build.cpp"
#include "platforms\win32\fat12\build.cpp"
#include "projects\simulator\build.cpp"
#include "projects\demos\directx\build.cpp"
#include "projects\handmade_hero\build.cpp"
#include "projects\demos\imgui\build.cpp"
#include "projects\ray_tracer\build.cpp"
#include "projects\x86_kernel\boot_sector\build.cpp"
#include "projects\x86_kernel\bootloader\build.cpp"
#include "projects\x86_kernel\kernel\build.cpp"
#include "projects\x86_kernel\build.cpp"

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
    {"x86_kernel_tests", &BuildX86KernelTests, NULL, NULL, NULL},
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