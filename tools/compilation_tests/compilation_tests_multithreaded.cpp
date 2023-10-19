// TODO:
// - a report of passed/failed tests is presented at the end
// - failed tests have extra error information provided
// - run the build in a separate directory for each thread/test job
//   so that no conflicts happen during the build due to file locking
// TODO:
// - build dependency graph of #include'ed files in compile targets
// - store the test results permanently, with info about dependencies
// - check on the last modification before running a test for a target
// - only rerun the test if necessary, meaning, if one of the target
//   dependencies changed since the last test run
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <strsafe.h>
#include <io.h>

#include "platform\base_types.h"
#include "platform\basic_defines.h"
#include "platform\console\console.h"

#include "platform\console\console.cpp"
#include "platform\strings.cpp"
#include "platform\processes\processes.cpp"

struct test_job
{
    char *TestOutputBuffer;
    u32 TestOutputBufferSize;
    char *TestCommand;
    b32 TestResult;
};

struct test_job_queue
{
    u32 JobCount;
    test_job *Jobs;
    volatile i64 NextJobIndex;
    volatile i64 TotalJobsDone;
};

console_context GlobalConsoleContext;

char *TestCommands[] =
{
    "build simulator",
    "build imgui_demo opengl2",
    "build imgui_demo dx11",
    "build ray_tracer 1_lane",
    "build ray_tracer 4_lanes",
    "build ray_tracer 8_lanes",
    "build handmade_hero",
    "build directx_demo debug",
    "build directx_demo release",
    "build lint",
    "build fetch_data",
    "build x86_kernel",
    "build fat12_tests",
    "build x86_kernel_tests",
};

void ProcessJob(test_job *Job)
{
    SECURITY_ATTRIBUTES ChildProcessOutputPipeSecturityAttributes = {};
    ChildProcessOutputPipeSecturityAttributes.bInheritHandle = TRUE;

    HANDLE PipeOutput;
    HANDLE PipeInput;
    CreatePipe(&PipeOutput, &PipeInput, &ChildProcessOutputPipeSecturityAttributes, 0);
    SetHandleInformation(PipeOutput, HANDLE_FLAG_INHERIT, 0);

    Job->TestResult = CreateProcessAndWait(Job->TestCommand, PipeInput, &GlobalConsoleContext);
    CloseHandle(PipeInput);

    char PipeOutputBuffer[1024] = {};
    DWORD BytesRead;
    ReadFile(PipeOutput, PipeOutputBuffer, 1024, &BytesRead, NULL);
    StringCchCat(Job->TestOutputBuffer, Job->TestOutputBufferSize, PipeOutputBuffer);
}

DWORD WINAPI
WorkerThreadEntry(void *Parameter)
{
    test_job_queue *JobQueue = (test_job_queue *)Parameter;

    while (JobQueue->TotalJobsDone < JobQueue->JobCount)
    {
        u64 JobIndex = InterlockedExchangeAdd64(&JobQueue->NextJobIndex, 1);
        if (JobIndex >= JobQueue->JobCount)
        {
            return TRUE;
        }

        ProcessJob(&JobQueue->Jobs[JobIndex]);
        InterlockedExchangeAdd64(&JobQueue->TotalJobsDone, 1);
    }

    return TRUE;
}

i32 main(i32 argc, char **argv)
{
    test_job_queue JobQueue;
    JobQueue.JobCount = ArrayCount(TestCommands);
    JobQueue.NextJobIndex = 0;
    JobQueue.TotalJobsDone = 0;
    JobQueue.Jobs = (test_job *)malloc(JobQueue.JobCount * sizeof(test_job));

    for (u32 JobIndex = 0; JobIndex < JobQueue.JobCount; JobIndex++)
    {
        test_job *Job = &JobQueue.Jobs[JobIndex];
        ZeroMemory(Job, sizeof(test_job));

        Job->TestCommand = TestCommands[JobIndex];
        Job->TestResult = FALSE;
        Job->TestOutputBufferSize = 1024;
        Job->TestOutputBuffer = (char *)malloc(Job->TestOutputBufferSize);
        ZeroMemory(Job->TestOutputBuffer, Job->TestOutputBufferSize);
    }

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    u32 ThreadCount = (u8)SystemInfo.dwNumberOfProcessors;
    // u32 ThreadCount = 1;

    MemoryBarrier();

    for (u32 ThreadIndex = 0; ThreadIndex < ThreadCount; ThreadIndex++)
    {
        DWORD ThreadId;
        HANDLE ThreadHandle = CreateThread(0, 0, WorkerThreadEntry, &JobQueue, 0, &ThreadId);
        CloseHandle(ThreadHandle);
    }

    while (JobQueue.TotalJobsDone < JobQueue.JobCount) {}

    b32 AllTestsSucceeded = TRUE;

    InitializeConsole(&GlobalConsoleContext);

    for (u32 JobIndex = 0; JobIndex < JobQueue.JobCount; JobIndex++)
    {
        test_job *Job = &JobQueue.Jobs[JobIndex];

        printf("\n");
        fflush(stdout);
        ConsoleSwitchColor(&GlobalConsoleContext, BACKGROUND_BLUE);
        printf("> %s", Job->TestCommand);
        fflush(stdout);
        ConsoleResetColor(&GlobalConsoleContext);
        printf("\n");
        fflush(stdout);

        printf("%s", Job->TestOutputBuffer);

        if (Job->TestResult)
        {
            ConsolePrintColored("INFO: test succeeded.\n", &GlobalConsoleContext, FOREGROUND_GREEN);
        }
        else
        {
            ConsolePrintColored("ERROR: test failed.\n", &GlobalConsoleContext, FOREGROUND_RED);
            AllTestsSucceeded = FALSE;
        }
    }

    if (AllTestsSucceeded)
    {
        ConsolePrintColored
        (
            "\n==========================\n"
            "INFO: all tests succeeded.\n"
            "==========================\n",
            &GlobalConsoleContext,
            FOREGROUND_GREEN
        );
    }

    for (u32 JobIndex = 0; JobIndex < JobQueue.JobCount; JobIndex++)
    {
        free(JobQueue.Jobs[JobIndex].TestOutputBuffer);
    }
    free(JobQueue.Jobs);

    return 0;
}