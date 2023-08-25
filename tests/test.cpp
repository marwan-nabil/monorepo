#include <Windows.h>
#include <stdint.h>
#include <io.h>
#include <stdio.h>

#include "..\miscellaneous\base_types.h"

HANDLE ConsoleHandle;
WORD OriginalConsoleAttributes;

void CompilationTest(char *TestCommand)
{
    printf("\n> %s\n", TestCommand);
    fflush(stdout);

    PROCESS_INFORMATION TestProcessProcessInfo = {};

    STARTUPINFO TestProcessStartupInfo = {};
    TestProcessStartupInfo.cb = sizeof(TestProcessStartupInfo);
    TestProcessStartupInfo.dwFlags = STARTF_USESTDHANDLES;
    TestProcessStartupInfo.hStdOutput = (HANDLE)_get_osfhandle(_fileno(stdout));
    TestProcessStartupInfo.hStdError = (HANDLE)_get_osfhandle(_fileno(stdout));

    BOOL CreateSucceeded = CreateProcess
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

    if (CreateSucceeded == false)
    {
        SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED);
        printf("ERROR: failed to create a test process, please debug the test system.\n");
        fflush(stdout);
        SetConsoleTextAttribute(ConsoleHandle, OriginalConsoleAttributes);
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
            SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED);
            printf("ERROR: test failed.\n");
            fflush(stdout);
            SetConsoleTextAttribute(ConsoleHandle, OriginalConsoleAttributes);
            exit(1);
        }
    }

    SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_GREEN);
    printf("INFO: test succeeded.\n");
    fflush(stdout);
    SetConsoleTextAttribute(ConsoleHandle, OriginalConsoleAttributes);
}

int main(int argc, char **argv)
{
    ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleInfo);

    OriginalConsoleAttributes = ConsoleInfo.wAttributes;

    SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_BLUE);
    printf("============= Compilation Tests =============\n");
    printf("These tests compile all available build targets to make sure\n");
    printf("that refactoring doesn't break the build of any target.\n");
    SetConsoleTextAttribute(ConsoleHandle, OriginalConsoleAttributes);

    CompilationTest("build build");

    CompilationTest("build simulator optimized");
    CompilationTest("build simulator non_optimized");

    CompilationTest("build ray_tracer optimized 1_lane");
    CompilationTest("build ray_tracer optimized 4_lanes");
    CompilationTest("build ray_tracer non_optimized 1_lane");
    CompilationTest("build ray_tracer non_optimized 4_lanes");

    SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_GREEN);
    printf("\n==========================\n");
    printf("INFO: all tests succeeded.\n");
    printf("==========================\n");
    fflush(stdout);
    SetConsoleTextAttribute(ConsoleHandle, OriginalConsoleAttributes);

    return 0;
}
