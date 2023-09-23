#include <Windows.h>
#include <stdint.h>
#include <io.h>
#include <stdio.h>

#include "..\miscellaneous\base_types.h"

#include "..\miscellaneous\console.cpp"
#include "compilation_tests.cpp"

void DisplayHelp()
{
    printf("INFO: Available test targets:\n");
    printf("          test compilation\n");
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