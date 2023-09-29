b32 CreateProcessAndWait(char *CommandLine)
{
    PROCESS_INFORMATION ProcessInfo = {};

    STARTUPINFO StartupInfo = {};
    StartupInfo.cb = sizeof(StartupInfo);
    StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    StartupInfo.hStdOutput = (HANDLE)_get_osfhandle(_fileno(stdout));
    StartupInfo.hStdError = (HANDLE)_get_osfhandle(_fileno(stdout));

    b32 CreateSucceeded = CreateProcess
    (
        NULL,
        CommandLine,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &StartupInfo,
        &ProcessInfo
    );

    if (CreateSucceeded == FALSE)
    {
        ConsolePrintColored("ERROR: failed to create a sub-process, please debug.\n", FOREGROUND_RED);
        return FALSE;
    }
    else
    {
        WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

        DWORD ProcessExitCode;
        GetExitCodeProcess(ProcessInfo.hProcess, &ProcessExitCode);

        CloseHandle(ProcessInfo.hProcess);
        CloseHandle(ProcessInfo.hThread);

        if (ProcessExitCode != 0)
        {
            ConsolePrintColored("ERROR: sub-process terminated with non zero error code.\n", FOREGROUND_RED);
            return FALSE;
        }
    }

    return TRUE;
}