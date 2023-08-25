#include <Windows.h>
#include <stdint.h>
#include <io.h>
#include <stdio.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\assertions.h"
#include "..\miscellaneous\basic_defines.h"

FILE *OutputFile;

void CompilationTest(char *TestCommand)
{
    fprintf(OutputFile, "================================================\n");
    fprintf(OutputFile, "> %s\n", TestCommand);

    PROCESS_INFORMATION TestProcessProcessInfo = {};
    STARTUPINFO TestProcessStartupInfo = {};
    TestProcessStartupInfo.cb = sizeof(TestProcessStartupInfo);
    TestProcessStartupInfo.dwFlags = STARTF_USESTDHANDLES;
    TestProcessStartupInfo.hStdOutput = (HANDLE)_get_osfhandle(_fileno(OutputFile));
    TestProcessStartupInfo.hStdError = (HANDLE)_get_osfhandle(_fileno(OutputFile));

    BOOL CreateSucceeded = CreateProcess
    (
        NULL,
        TestCommand,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &TestProcessStartupInfo,
        &TestProcessProcessInfo
    );

    if (CreateSucceeded == false)
    {
        fprintf(OutputFile, "ERROR: failed to create a test process, please debug the test system.\n");
    }
    else
    {
        WaitForSingleObject(TestProcessProcessInfo.hProcess, INFINITE);
        CloseHandle(TestProcessProcessInfo.hProcess);
        CloseHandle(TestProcessProcessInfo.hThread);
    }

    fprintf(OutputFile, "\n");
}

int main(int argc, char **argv)
{
    OutputFile = fopen("test_results.txt", "wb");
    if (OutputFile == NULL)
    {
        printf("ERROR: unable to create test_results.txt file.\n");
    }

    CompilationTest("build build");

    CompilationTest("build simulator optimized");
    CompilationTest("build simulator non_optimized");

    CompilationTest("build ray_tracer optimized 1_lane");
    CompilationTest("build ray_tracer optimized 4_lanes");
    CompilationTest("build ray_tracer non_optimized 1_lane");
    CompilationTest("build ray_tracer non_optimized 4_lanes");

    return 0;
}
