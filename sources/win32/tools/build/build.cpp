#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>

#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\console\console.h"
#include "sources\win32\file_system\folders.h"
#include "sources\win32\strings\path_handling.h"

#include "build.h"
#include "target_configuration.h"

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

    InitializeConsole();

    if (argc < 2)
    {
        ConsolePrintColored("ERROR: No build target.\n", FOREGROUND_RED);
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
            ConsoleSwitchColor(FOREGROUND_RED);
            printf("ERROR: invalid build target \"%s\".\n", argv[1]);
            ConsoleResetColor();
            DisplayHelp();
            BuildSuccess = FALSE;
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