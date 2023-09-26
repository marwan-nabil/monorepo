#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <io.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\assertions.h"
#include "..\miscellaneous\basic_defines.h"

#include "..\fat12\fat12.h"

#include "..\miscellaneous\console.cpp"
#include "..\miscellaneous\strings.cpp"

#include "compilation_tests.cpp"

#include "..\fat12\fat12.cpp"
#include "..\fat12\test.cpp"

void DisplayHelp()
{
    printf("INFO: Available test targets:\n");
    printf("          test compilation\n");
    printf("          test fat12\n");
}

int main(int argc, char **argv)
{
    InitializeConsole();

    if (argc < 2)
    {
        ConsolePrintColored("ERROR: No test target.\n", FOREGROUND_RED);
        DisplayHelp();
        return 1;
    }
    else if (strcmp(argv[1], "compilation") == 0)
    {
        RunCompilationTests();
    }
    else if (strcmp(argv[1], "fat12") == 0)
    {
        TestFat12();
    }
    else
    {
        ConsolePrintColored("ERROR: invalid test target.\n", FOREGROUND_RED);
        DisplayHelp();
        return 1;
    }

    ConsoleSwitchColor(FOREGROUND_GREEN);
    printf("\n==========================\n");
    printf("INFO: all tests succeeded.\n");
    printf("==========================\n");
    fflush(stdout);
    ConsoleResetColor();

    return 0;
}