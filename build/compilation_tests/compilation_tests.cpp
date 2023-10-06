#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>

#include "..\..\platform\base_types.h"
#include "..\..\platform\basic_defines.h"

#include "..\..\platform\console\console.cpp"
#include "..\..\platform\strings\strings.cpp"
#include "..\..\platform\processes\processes.cpp"

void CompilationTest(char *TestCommand)
{
    printf("\n");
    ConsoleSwitchColor(BACKGROUND_BLUE);
    printf("> %s", TestCommand);
    ConsoleResetColor();
    printf("\n");
    fflush(stdout);

    b32 Result = CreateProcessAndWait(TestCommand);
    if (Result)
    {
        ConsolePrintColored("INFO: test succeeded.\n", FOREGROUND_GREEN);
    }
    else
    {
        ConsolePrintColored("ERROR: test failed.\n", FOREGROUND_RED);
        exit(1);
    }
}

i32 main(i32 argc, char **argv)
{
    InitializeConsole();

    CompilationTest("build simulator");
    CompilationTest("build imgui_demo opengl2");
    CompilationTest("build imgui_demo dx11");
    CompilationTest("build ray_tracer optimized 1_lane");
    CompilationTest("build ray_tracer optimized 4_lanes");
    CompilationTest("build ray_tracer non_optimized 1_lane");
    CompilationTest("build ray_tracer non_optimized 4_lanes");
    CompilationTest("build handmade_hero");
    CompilationTest("build directx_demo debug");
    CompilationTest("build directx_demo release");
    CompilationTest("build lint");
    CompilationTest("build x86_kernel");
    CompilationTest("build fat12_tests");
    CompilationTest("build x86_kernel_tests");
    CompilationTest("build clean");

    ConsolePrintColored
    (
        "\n==========================\n"
        "INFO: all tests succeeded.\n"
        "==========================\n",
        FOREGROUND_GREEN
    );
}