b32 CleanExtensionFromDirectory(const char *ExtensionToClean, const char *DirectoryPath)
{
    char FilesWildcard[MAX_PATH] = {};
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
            ConsolePrintColored("ERROR: FindFirstFileA() failed.\n", FOREGROUND_RED);
            return FALSE;
        }
    }
    else
    {
        do
        {
            if ((FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                char FoundFilePath[512] = {};
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

                    ConsoleSwitchColor(FOREGROUND_BLUE);
                    printf
                    (
                        "WARNING: Cannot delete the file %s. System error code for DeleteFile(): %lu == %s",
                        FoundFilePath, LastError, (const char *)ErrorMessageFromSystem
                    );
                    fflush(stdout);
                    ConsoleResetColor();

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
            ConsoleSwitchColor(FOREGROUND_RED);
            printf("ERROR: cleanup process did not finish properly, please debug.\n");
            printf("ERROR: last error code is %d\n", LastErrorCode);
            printf("ERROR: extension with error is %s\n", ExtensionToClean);
            fflush(stdout);
            ConsoleResetColor();
            return FALSE;
        }
    }

    FindClose(FindHandle);
    return TRUE;
}