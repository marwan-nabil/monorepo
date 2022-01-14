#pragma comment(lib, "Shell32.lib")

#include <Windows.h>

#include "platform_types.h"
#include "utility_macros.h"

i32
WinMain
(
    HINSTANCE Instance, HINSTANCE PrevInstance,
    LPSTR CmdLine, i32 ShowCmd
)
{
    const char *SourceDirectoryAbsolutePath = "W:\\thing";

    size_t SourceDirectoryAbsolutePathStringLength = 0;
    for (u32 StringIndex = 0; StringIndex < MAX_PATH; StringIndex++)
    {
        if (SourceDirectoryAbsolutePath[StringIndex] == '\0')
        {
            break;
        }
        else
        {
            SourceDirectoryAbsolutePathStringLength++;
        }
    }

    if (SourceDirectoryAbsolutePathStringLength > (MAX_PATH - 3))
    {
        OutputDebugStringA("Path that will be used to build is too long!");
        goto end;
    }

    char *ExtensionsOfFilesToBeCleaned[3] = 
    {
        "obj", "pdb", "exe"
    };

    for (u32 ExtensionToBeCleanedIndex = 0; ExtensionToBeCleanedIndex < 3; ExtensionToBeCleanedIndex++)
    {
        char FilePathPatternOfFilesToBeDeleted[MAX_PATH];

        for (u32 StringIndex = 0; StringIndex < SourceDirectoryAbsolutePathStringLength; StringIndex++)
        {
            FilePathPatternOfFilesToBeDeleted[StringIndex] = SourceDirectoryAbsolutePath[StringIndex];
        }

        u32 ExtensionStringLength = 0;
        for (u32 StringIndex = 0; StringIndex < 5; StringIndex++)
        {
            if (ExtensionsOfFilesToBeCleaned[ExtensionToBeCleanedIndex][StringIndex] == '\0')
            {
                ExtensionStringLength = StringIndex;
                break;
            }
            else
            {
                FilePathPatternOfFilesToBeDeleted[SourceDirectoryAbsolutePathStringLength + StringIndex] = 
                    ExtensionsOfFilesToBeCleaned[ExtensionToBeCleanedIndex][StringIndex];
            }
        }

        FilePathPatternOfFilesToBeDeleted[SourceDirectoryAbsolutePathStringLength + ExtensionStringLength] = '\0';
        FilePathPatternOfFilesToBeDeleted[SourceDirectoryAbsolutePathStringLength + ExtensionStringLength + 1] = '\0';
    }
    
    StringCchCopyA(PathPatternBuffer, MAX_PATH, SourceDirectoryAbsolutePath);
    StringCchCatA(PathPatternBuffer, MAX_PATH, "\\*.");

    WIN32_FIND_DATAA FindOperationData;
    HANDLE FindHandle = FindFirstFileA(PathPatternBuffer, &FindOperationData);
    Assert(INVALID_HANDLE_VALUE != FindHandle);
    
    do
    {
        char AbsoluteFilePathBuffer[512];
        ZeroMemory(AbsoluteFilePathBuffer, 512);
        StringCchCopyA(AbsoluteFilePathBuffer, MAX_PATH, SourceDirectoryAbsolutePath);
        StringCchCatA(AbsoluteFilePathBuffer, MAX_PATH, "\\");

        if (FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            StringCchCatA(AbsoluteFilePathBuffer, MAX_PATH, FindOperationData.cFileName);
            StringCchLengthA(AbsoluteFilePathBuffer, MAX_PATH, &LastStringLength);
            AbsoluteFilePathBuffer[LastStringLength] = 0;
            AbsoluteFilePathBuffer[LastStringLength + 1] = 0;

            SHFILEOPSTRUCTA ShellFileOperation;
            ShellFileOperation.hwnd = 0;
            ShellFileOperation.wFunc = FO_DELETE;
            ShellFileOperation.pFrom = AbsoluteFilePathBuffer;
            ShellFileOperation.pTo = 0;
            ShellFileOperation.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_ALLOWUNDO;
            ShellFileOperation.fAnyOperationsAborted = 0;
            ShellFileOperation.hNameMappings = 0;
            ShellFileOperation.lpszProgressTitle = 0;
            SHFileOperationA(&ShellFileOperation);

            // TODO: wait until shell operation finishes
        }
        else
        {
            StringCchCatA(AbsoluteFilePathBuffer, MAX_PATH, FindOperationData.cFileName);
            DeleteFileA(AbsoluteFilePathBuffer);
        }
    } while (FindNextFileA(FindHandle, &FindOperationData) != 0);

    DWORD LastError = GetLastError();
    Assert(LastError == ERROR_NO_MORE_FILES);
    FindClose(FindHandle);

    STARTUPINFO CompilerProcessStartupInfo = {};
    CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
    PROCESS_INFORMATION CompilerProcessProcessInfo = {};

    // TODO: craft command line arguments to the compiler
    // TODO: determine the compiler path
#if 0
    BOOL CreateSucceeded = CreateProcess
    (
        NULL,   // No module name (use command line)
        argv[1],        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi
    );

    if (!CreateSucceeded)
    {
        return -1;
    }

    WaitForSingleObject(CompilerProcessProcessInfo.hProcess, INFINITE);

    CloseHandle(CompilerProcessProcessInfo.hProcess);
    CloseHandle(CompilerProcessProcessInfo.hThread);
#endif

end:
    return 0;
}
