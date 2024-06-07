#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include <math.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\shell\console.h"
#include "folders.h"

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

b32 DoesDirectoryExist(const char *DirectoryPath)
{
    DWORD FileAttributes = GetFileAttributes(DirectoryPath);
    if
    (
        (FileAttributes != INVALID_FILE_ATTRIBUTES) &&
        (FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    )
    {
        return TRUE;
    }
    return FALSE;
}

void DeleteDirectoryCompletely(const char *DirectoryPath)
{
    SHFILEOPSTRUCT ShellOperation;
    ShellOperation.hwnd = NULL;
    ShellOperation.wFunc = FO_DELETE;
    ShellOperation.pFrom = DirectoryPath;
    ShellOperation.pTo = "";
    ShellOperation.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    ShellOperation.fAnyOperationsAborted = FALSE;
    ShellOperation.hNameMappings = 0;
    ShellOperation.lpszProgressTitle = "";

    SHFileOperation(&ShellOperation);
}

void EmptyDirectory(const char *DirectoryPath)
{
    char SearchPattern[1024] = {};
    StringCchCatA(SearchPattern, 1024, DirectoryPath);
    StringCchCatA(SearchPattern, 1024, "\\*");

    WIN32_FIND_DATAA FindOperationData;
    HANDLE FindHandle = FindFirstFileA(SearchPattern, &FindOperationData);
    if (FindHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if
                (
                    (strcmp(FindOperationData.cFileName, ".") != 0) &&
                    (strcmp(FindOperationData.cFileName, "..") != 0)
                )
                {
                    char SubDirectoryPath[1024] = {};
                    StringCchCatA(SubDirectoryPath, 1024, DirectoryPath);
                    StringCchCatA(SubDirectoryPath, 1024, "\\");
                    StringCchCatA(SubDirectoryPath, 1024, FindOperationData.cFileName);
                    DeleteDirectoryCompletely(SubDirectoryPath);
                }
            }
            else
            {
                char FoundFilePath[1024] = {};
                StringCchCatA(FoundFilePath, 1024, DirectoryPath);
                StringCchCatA(FoundFilePath, 1024, "\\");
                StringCchCatA(FoundFilePath, 1024, FindOperationData.cFileName);

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
                    fflush(stdout);
                    LocalFree(ErrorMessageFromSystem);
                }
                else
                {
                    printf("INFO: Deleted file: %s\n", FoundFilePath);
                }
            }
        } while (FindNextFileA(FindHandle, &FindOperationData) != 0);
    }
}