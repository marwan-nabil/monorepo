#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>

#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\console\console.h"

b32 CreateProcessAndWait(char *CommandLine, HANDLE ProcessOutput)
{
    PROCESS_INFORMATION ProcessInfo = {};

    STARTUPINFO StartupInfo = {};
    StartupInfo.cb = sizeof(StartupInfo);
    StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    StartupInfo.hStdOutput = ProcessOutput;
    StartupInfo.hStdError = ProcessOutput;

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
        DWORD LastError = GetLastError();
        LPVOID ErrorMessageFromSystem;
        FormatMessage
        (
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            LastError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&ErrorMessageFromSystem,
            0,
            NULL
        );

        ConsoleSwitchColor(FOREGROUND_RED);
        printf
        (
            "ERROR: failed to create a sub-process, system error code: %lu == %s",
            LastError, (const char *)ErrorMessageFromSystem
        );
        fflush(stdout);
        ConsoleResetColor();

        LocalFree(ErrorMessageFromSystem);
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
            return FALSE;
        }
    }

    return TRUE;
}

b32 CreateProcessAndWait(char *CommandLine)
{
    return CreateProcessAndWait(CommandLine, (HANDLE)_get_osfhandle(_fileno(stdout)));
}