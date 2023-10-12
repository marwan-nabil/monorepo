#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>

#include "..\..\platform\base_types.h"
#include "..\..\platform\basic_defines.h"
#include "..\..\platform\console\console.h"

#include "..\..\platform\console\console.cpp"
#include "..\..\platform\strings\strings.cpp"

console_context GlobalConsoleContext;

i32 main(i32 argc, char **argv)
{
    InitializeConsole(&GlobalConsoleContext);

    ConsolePrintColored
    (
        "\n==========================\n"
        "INFO: all tests succeeded.\n"
        "==========================\n",
        &GlobalConsoleContext,
        FOREGROUND_GREEN
    );
}