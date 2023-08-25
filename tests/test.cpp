#include <Windows.h>
#include <stdint.h>
#include <io.h>
#include <stdio.h>

#include "..\miscellaneous\base_types.h"

#include "console.cpp"
#include "compilation_tests.cpp"
#include "software_rendering_tests.cpp"

int main(int argc, char **argv)
{
    InitializeConsole();

    CompilationTests();

    // SoftwareRenderingTests();

    ConsoleSwitchColor(FOREGROUND_GREEN);
    printf("\n==========================\n");
    printf("INFO: all tests succeeded.\n");
    printf("==========================\n");
    fflush(stdout);
    ConsoleResetColor();

    return 0;
}