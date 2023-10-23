#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>

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

#include "target_mappings.cpp"

console_context GlobalConsoleContext;

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
        const char *ExtensionsToClean[] =
        {
            "obj", "pdb", "log", "ilk", "sln", "bmp", "txt", "ini",
            "dll", "exp", "lib", "map", "hmi", "cso", "lock", "exe",
            "img", "h", "err"
        };

        for (u32 ExtensionIndex = 0; ExtensionIndex < ArrayCount(ExtensionsToClean); ExtensionIndex++)
        {
            BuildSuccess = CleanExtensionFromDirectory
            (
                ExtensionsToClean[ExtensionIndex],
                BuildContext.OutputDirectoryPath,
                &GlobalConsoleContext
            );
        }
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        DisplayHelp();
        BuildSuccess = TRUE;
    }
    else
    {
        u32 TargetIndex;
        for (TargetIndex = 0; TargetIndex < ArrayCount(BuildTargetMappings); TargetIndex++)
        {
            if (strcmp(argv[1], BuildTargetMappings[TargetIndex].TargetName) == 0)
            {
                BuildSuccess = BuildTargetMappings[TargetIndex].BuildFunction(&BuildContext);
                break;
            }
        }

        if (TargetIndex == ArrayCount(BuildTargetMappings))
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