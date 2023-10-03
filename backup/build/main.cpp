#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>

#include "..\platform\base_types.h"
#include "..\platform\basic_defines.h"
#include "..\platform\file_system\files.h"
#include "..\drivers\fat12\fat12.h"
#include "build.h"

#include "..\math\scalar_conversions.cpp"
#include "..\platform\strings\strings.cpp"
#include "..\platform\console\console.cpp"
#include "..\platform\file_system\files.cpp"
#include "..\platform\file_system\folders.cpp"
#include "..\platform\processes\processes.cpp"
#include "..\drivers\fat12\fat12_get.cpp"
#include "..\drivers\fat12\fat12_set.cpp"
#include "..\drivers\fat12\fat12_access_layer.cpp"
#include "build_helpers.cpp"
#include "build_targets.cpp"

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

    InitializeConsole();

    if (argc < 2)
    {
        ConsolePrintColored("ERROR: No build target.\n", FOREGROUND_RED);
        DisplayHelp();
        return 1;
    }

    if (strcmp(argv[1], "clean") == 0)
    {
        const char *ExtensionsToClean[] =
        {
            "obj", /* "pdb", */ "log", "ilk", "sln", "bmp", "txt", "ini",
            "dll", "exp", "lib", "map", "hmi", "cso", "lock", "exe",
            "img", "h"
        };

        for (u32 ExtensionIndex = 0; ExtensionIndex < ArrayCount(ExtensionsToClean); ExtensionIndex++)
        {
            BuildSuccess = CleanExtensionFromDirectory
            (
                ExtensionsToClean[ExtensionIndex],
                BuildContext.OutputDirectoryPath
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
        u32 TargetIndex = 0;
        for (; TargetIndex < ArrayCount(BuildTargetMappings); TargetIndex++)
        {
            if (strcmp(argv[1], BuildTargetMappings[TargetIndex].TargetName) == 0)
            {
                BuildSuccess = BuildTargetMappings[TargetIndex].BuildFunction(&BuildContext);
                break;
            }
        }

        if (TargetIndex == ArrayCount(BuildTargetMappings))
        {
            ConsoleSwitchColor(FOREGROUND_RED);
            printf("ERROR: invalid build target \"%s\".\n", argv[1]);
            ConsoleResetColor();
            DisplayHelp();
            BuildSuccess = FALSE;
        }
    }

    if (BuildSuccess)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}