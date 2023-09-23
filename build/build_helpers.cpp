u32 CleanExtensionFromDirectory(const char *ExtensionToClean, const char *DirectoryPath)
{
    char FilesWildcard[MAX_PATH];
    ZeroMemory(FilesWildcard, ArrayCount(FilesWildcard));
    StringCchCatA(FilesWildcard, MAX_PATH, DirectoryPath);
    StringCchCatA(FilesWildcard, MAX_PATH, "\\*.");
    StringCchCatA(FilesWildcard, MAX_PATH, ExtensionToClean);

    WIN32_FIND_DATAA FindOperationData;
    HANDLE FindHandle = FindFirstFileA(FilesWildcard, &FindOperationData);

    if (FindHandle == INVALID_HANDLE_VALUE)
    {
        DWORD LastError = GetLastError();
        if (LastError != ERROR_FILE_NOT_FOUND)
        {
            printf("ERROR: FindFirstFileA() failed.\n");
            return 1;
        }
    }
    else
    {
        do
        {
            if ((FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                char FoundFilePath[512];
                ZeroMemory(FoundFilePath, 512);
                StringCchCatA(FoundFilePath, MAX_PATH, DirectoryPath);
                StringCchCatA(FoundFilePath, MAX_PATH, "\\");
                StringCchCatA(FoundFilePath, 512, FindOperationData.cFileName);

                b32 DeleteResult = DeleteFile(FoundFilePath);
                if (DeleteResult == 0)
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

                    printf
                    (
                        "WARNING: Cannot delete the file %s. System error code for DeleteFile(): %lu == %s",
                        FoundFilePath, LastError, (const char *)ErrorMessageFromSystem
                    );

                    LocalFree(ErrorMessageFromSystem);
                }
                else
                {
                    printf("INFO: Deleted file: %s\n", FoundFilePath);
                }
            }
        } while (FindNextFileA(FindHandle, &FindOperationData) != 0);

        DWORD LastErrorCode = GetLastError();
        if (LastErrorCode != ERROR_NO_MORE_FILES)
        {
            printf("ERROR: cleanup process did not finish properly, please debug.\n");
            printf("ERROR: last error code is %d\n", LastErrorCode);
            printf("ERROR: extension with error is %s\n", ExtensionToClean);
            return 1;
        }
    }

    FindClose(FindHandle);

    return 0;
}

b32 CompileShader
(
    char *CompilerFlags,
    char *SourcesString,
    char *OutputBinaryPath
)
{
    STARTUPINFO CompilerProcessStartupInfo = {};
    CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
    PROCESS_INFORMATION CompilerProcessProcessInfo = {};

    char CompilerCommand[1024];
    ZeroMemory(CompilerCommand, ArrayCount(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "fxc.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fo \"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), SourcesString);

    b32 CreateSucceeded = CreateProcess
    (
        NULL,
        CompilerCommand,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &CompilerProcessStartupInfo,
        &CompilerProcessProcessInfo
    );

    if (CreateSucceeded == FALSE)
    {
        printf("ERROR: failed to create the compiler process, please debug the build system.\n");
        fflush(stdout);
        return FALSE;
    }
    else
    {
        WaitForSingleObject(CompilerProcessProcessInfo.hProcess, INFINITE);

        DWORD ProcessExitCode;
        GetExitCodeProcess(CompilerProcessProcessInfo.hProcess, &ProcessExitCode);

        CloseHandle(CompilerProcessProcessInfo.hProcess);
        CloseHandle(CompilerProcessProcessInfo.hThread);

        if (ProcessExitCode != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

b32 InvokeCompiler
(
    char *CompilerFlags,
    char *SourcesString,
    char *OutputBinaryPath,
    char *LinkerFlags
)
{
    STARTUPINFO CompilerProcessStartupInfo = {};
    CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
    PROCESS_INFORMATION CompilerProcessProcessInfo = {};

    char CompilerCommand[1024];
    ZeroMemory(CompilerCommand, ArrayCount(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "cl.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), SourcesString);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fe:\"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "/link ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), LinkerFlags);

    b32 CreateSucceeded = CreateProcess
    (
        NULL,
        CompilerCommand,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &CompilerProcessStartupInfo,
        &CompilerProcessProcessInfo
    );

    if (CreateSucceeded == FALSE)
    {
        printf("ERROR: failed to create the compiler process, please debug the build system.\n");
        fflush(stdout);
        return FALSE;
    }
    else
    {
        WaitForSingleObject(CompilerProcessProcessInfo.hProcess, INFINITE);

        DWORD ProcessExitCode;
        GetExitCodeProcess(CompilerProcessProcessInfo.hProcess, &ProcessExitCode);

        CloseHandle(CompilerProcessProcessInfo.hProcess);
        CloseHandle(CompilerProcessProcessInfo.hThread);

        if (ProcessExitCode != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}