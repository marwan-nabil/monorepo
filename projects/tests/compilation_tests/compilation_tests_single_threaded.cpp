#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <strsafe.h>
#include <io.h>

#include "libraries\win32\base_types.h"
#include "libraries\win32\basic_defines.h"
#include "libraries\win32\console\console.h"

#include "libraries\win32\console\console.cpp"
#include "libraries\win32\strings\strings.cpp"
#include "libraries\win32\processes\processes.cpp"

console_context GlobalConsoleContext;

void CompilationTest(char *TestCommand)
{
    printf("\n");
    ConsoleSwitchColor(&GlobalConsoleContext, BACKGROUND_BLUE);
    printf("> %s", TestCommand);
    ConsoleResetColor(&GlobalConsoleContext);
    printf("\n");
    fflush(stdout);

    b32 Result = CreateProcessAndWait(TestCommand, &GlobalConsoleContext);
    if (Result)
    {
        ConsolePrintColored("INFO: test succeeded.\n", &GlobalConsoleContext, FOREGROUND_GREEN);
    }
    else
    {
        ConsolePrintColored("ERROR: test failed.\n", &GlobalConsoleContext, FOREGROUND_RED);
        exit(1);
    }
}

i32 main(i32 argc, char **argv)
{
    InitializeConsole(&GlobalConsoleContext);

    CompilationTest("build simulator");
    CompilationTest("build imgui_demo opengl2");
    CompilationTest("build imgui_demo dx11");
    CompilationTest("build ray_tracer 1_lane");
    CompilationTest("build ray_tracer 4_lanes");
    CompilationTest("build ray_tracer 8_lanes");
    CompilationTest("build handmade_hero");
    CompilationTest("build directx_demo debug");
    CompilationTest("build directx_demo release");
    CompilationTest("build lint");
    CompilationTest("build fetch_data");
    CompilationTest("build fat12_tests");
    CompilationTest("build compilation_tests_multi_threaded");
    CompilationTest("build os_real");
    CompilationTest("build x86_kernel_tests");

    ConsolePrintColored
    (
        "\n==========================\n"
        "INFO: all tests succeeded.\n"
        "==========================\n",
        &GlobalConsoleContext,
        FOREGROUND_GREEN
    );
}