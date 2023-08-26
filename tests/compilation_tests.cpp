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

void CompilationTests()
{
    ConsoleSwitchColor(FOREGROUND_BLUE);
    printf("\n============= Compilation Tests =============\n");
    printf("These tests compile all available build targets to make sure\n");
    printf("that refactoring doesn't break the build of any target.\n");
    ConsoleResetColor();

    CompilationTest("build build");

    CompilationTest("build basic_windows_application");

    CompilationTest("build ray_tracer optimized 1_lane");
    CompilationTest("build ray_tracer optimized 4_lanes");
    CompilationTest("build ray_tracer non_optimized 1_lane");
    CompilationTest("build ray_tracer non_optimized 4_lanes");
}