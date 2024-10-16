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

void DisplayHelp()
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

    if (argc < 2)
    {
        ConsolePrintColored("ERROR: No build target.\n", FOREGROUND_RED);
        DisplayHelp();
        BuildSuccess = FALSE;
    }
    else
    {
        if (strcmp(argv[1], "clean") == 0)
        {
            for (u32 TargetIndex = 0; TargetIndex < ArrayCount(BuildTargetConfigurations); TargetIndex++)
            {
                char DirectoryName[1024] = {};
                StringCchCat(DirectoryName, ArrayCount(DirectoryName), BuildContext.EnvironmentInfo.OutputDirectoryPath);
                StringCchCat(DirectoryName, ArrayCount(DirectoryName), "\\");
                StringCchCat(DirectoryName, ArrayCount(DirectoryName), BuildTargetConfigurations[TargetIndex].TargetName);
                if (DoesDirectoryExist(DirectoryName))
                {
                    DeleteDirectoryCompletely(DirectoryName);
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
                    SetCurrentDirectory(BuildContext.EnvironmentInfo.RootDirectoryPath);
                }
            }
            else
            {
                ConsoleSwitchColor(FOREGROUND_RED);
                printf("ERROR: invalid build target \"%s\".\n", argv[1]);
                ConsoleResetColor();
                DisplayHelp();
                BuildSuccess = FALSE;
            }
        }
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