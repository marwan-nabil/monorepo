#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\assertions.h"
#include "..\miscellaneous\basic_defines.h"

#include "..\miscellaneous\console.cpp"
#include "..\miscellaneous\strings.cpp"

void CompilationTest(char *TestCommand)
{
    printf("\n");
    ConsoleSwitchColor(BACKGROUND_BLUE);
    printf("> %s", TestCommand);
    ConsoleResetColor();
    printf("\n");
    fflush(stdout);

    PROCESS_INFORMATION TestProcessProcessInfo = {};

    STARTUPINFO TestProcessStartupInfo = {};
    TestProcessStartupInfo.cb = sizeof(TestProcessStartupInfo);
    TestProcessStartupInfo.dwFlags = STARTF_USESTDHANDLES;
    TestProcessStartupInfo.hStdOutput = (HANDLE)_get_osfhandle(_fileno(stdout));
    TestProcessStartupInfo.hStdError = (HANDLE)_get_osfhandle(_fileno(stdout));

    b32 CreateSucceeded = CreateProcess
    (
        NULL,
        TestCommand,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &TestProcessStartupInfo,
        &TestProcessProcessInfo
    );

    if (CreateSucceeded == FALSE)
    {
        ConsolePrintColored("ERROR: failed to create a test process, please debug the test system.\n", FOREGROUND_RED);
        exit(1);
    }
    else
    {
        WaitForSingleObject(TestProcessProcessInfo.hProcess, INFINITE);

        DWORD ProcessExitCode;
        GetExitCodeProcess(TestProcessProcessInfo.hProcess, &ProcessExitCode);

        CloseHandle(TestProcessProcessInfo.hProcess);
        CloseHandle(TestProcessProcessInfo.hThread);

        if (ProcessExitCode != 0)
        {
            ConsolePrintColored("ERROR: test failed.\n", FOREGROUND_RED);
            exit(1);
        }
    }

    ConsolePrintColored("INFO: test succeeded.\n", FOREGROUND_GREEN);
}

i32 main(i32 argc, char **argv)
{
    InitializeConsole();

    CompilationTest("build build");
    CompilationTest("build basic_app");
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
    CompilationTest("build clean");

    ConsoleSwitchColor(FOREGROUND_GREEN);
    printf("\n==========================\n");
    printf("INFO: all tests succeeded.\n");
    printf("==========================\n");
    fflush(stdout);
    ConsoleResetColor();
}