#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>

#include "platform\base_types.h"
#include "platform\basic_defines.h"
#include "platform\console\console.h"
#include "platform\file_system\files.h"
#include "platform\file_system\path_handling.h"
#include "drivers\fat12\fat12.h"

#include "build.h"

#include "math\scalar_conversions.cpp"
#include "platform\strings.cpp"
#include "platform\console\console.cpp"
#include "platform\file_system\files.cpp"
#include "platform\file_system\folders.cpp"
#include "platform\file_system\path_handling.cpp"
#include "platform\processes\processes.cpp"
#include "drivers\fat12\fat12_get.cpp"
#include "drivers\fat12\fat12_set.cpp"
#include "drivers\fat12\fat12_interface.cpp"

#include "build_helpers.cpp"

#include "tools\lint\build.cpp"
#include "tools\fetch_data\build.cpp"
#include "tools\compilation_tests\build.cpp"
#include "drivers\fat12\build.cpp"
#include "projects\simulator\build.cpp"
#include "projects\directx_demo\build.cpp"
#include "projects\handmade_hero\build.cpp"
#include "projects\imgui_demo\build.cpp"
#include "projects\ray_tracer\build.cpp"
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