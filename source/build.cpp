#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>

typedef uint8_t  b8;
typedef uint32_t b32;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define ArrayLength(Array) (sizeof(Array) / sizeof((Array)[0]))
#define Assert(Expression) {if(!(Expression)){ *(int *)0 = 0; }}

int main(int argc, char **argv)
{
    b32 CleanOnly = false;
    if
    (
        (argc == 2) && 
        (strcmp(argv[1], "clean") == 0)
    )
    {
        CleanOnly = true;
    }

    const char *RootDirectoryPath = "F:\\thing";

    char SourceDirectoryPath[MAX_PATH];
    ZeroMemory(SourceDirectoryPath, ArrayLength(SourceDirectoryPath));
    StringCchCatA(SourceDirectoryPath, ArrayLength(SourceDirectoryPath), RootDirectoryPath);
    StringCchCatA(SourceDirectoryPath, ArrayLength(SourceDirectoryPath), "\\source");

    char BinDirectoryPath[MAX_PATH];
    ZeroMemory(BinDirectoryPath, ArrayLength(BinDirectoryPath));
    StringCchCatA(BinDirectoryPath, ArrayLength(BinDirectoryPath), RootDirectoryPath);
    StringCchCatA(BinDirectoryPath, ArrayLength(BinDirectoryPath), "\\bin");

    const char *ExtensionsToClean[] = 
    {
        "obj", "pdb", "exe", "log", "ilk", "sln"
    };

    printf("\n====== Cleanup ======\n");

    for (u32 ExtensionIndex = 0; ExtensionIndex < ArrayLength(ExtensionsToClean); ExtensionIndex++)
    {
        char FilesWildcard[MAX_PATH];
        ZeroMemory(FilesWildcard, ArrayLength(FilesWildcard));
        StringCchCopyA(FilesWildcard, MAX_PATH, BinDirectoryPath);
        StringCchCatA(FilesWildcard, MAX_PATH, "\\*.");
        StringCchCatA(FilesWildcard, MAX_PATH, ExtensionsToClean[ExtensionIndex]);

        WIN32_FIND_DATAA FindOperationData;
        HANDLE FindHandle = FindFirstFileA(FilesWildcard, &FindOperationData);

        if(FindHandle == INVALID_HANDLE_VALUE)
        {
            if(GetLastError() != ERROR_FILE_NOT_FOUND)
            {
                printf("ERROR: FindFirstFileA() failed.\n");
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
                    StringCchCopyA(FoundFilePath, MAX_PATH, BinDirectoryPath);
                    StringCchCatA(FoundFilePath, MAX_PATH, "\\");
                    StringCchCatA(FoundFilePath, 512, FindOperationData.cFileName);

                    if (!DeleteFile(FoundFilePath))
                    {
                        DWORD LastError = GetLastError();

                        LPVOID ErrorMessageFromSystem;
                        FormatMessage
                        (
                            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL,
                            LastError,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPTSTR) &ErrorMessageFromSystem,
                            0,
                            NULL
                        );

                        printf
                        (
                            "WARNING: Cannot delete the file %s. System error code for DeleteFile(): %d == %s", 
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
                printf("ERROR: extension with error is %s\n", ExtensionsToClean[ExtensionIndex]);
            }
        }

        FindClose(FindHandle);
    }

    if (!CleanOnly)
    {
        printf("\n====== Compiler output ======\n");

        STARTUPINFO CompilerProcessStartupInfo = {};
        CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
        PROCESS_INFORMATION CompilerProcessProcessInfo = {};

        char MainTranslationUnitPath[MAX_PATH];
        ZeroMemory(MainTranslationUnitPath, ArrayLength(MainTranslationUnitPath));
        StringCchCatA(MainTranslationUnitPath, ArrayLength(MainTranslationUnitPath), SourceDirectoryPath);
        StringCchCatA(MainTranslationUnitPath, ArrayLength(MainTranslationUnitPath), "\\main.cpp");

        char CompilerCommand[1024];
        ZeroMemory(CompilerCommand, 1024);
        StringCchCatA(CompilerCommand, 1024, "cl.exe ");
        StringCchCatA(CompilerCommand, 1024, "-nologo -Zi -FC -Od -Oi -GR- -EHa- -Gm- -MTd ");
        StringCchCatA(CompilerCommand, 1024, "-W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4996 -wd4018 ");
        StringCchCatA(CompilerCommand, 1024, MainTranslationUnitPath);
        StringCchCatA(CompilerCommand, 1024, " /link /incremental:no /subsystem:windows /opt:ref /Fe:");
        StringCchCatA(CompilerCommand, 1024, BinDirectoryPath);
        StringCchCatA(CompilerCommand, 1024, "\\main.exe ");
        StringCchCatA(CompilerCommand, 1024, "user32.lib gdi32.lib");

        BOOL CreateSucceeded = CreateProcess
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

        if (CreateSucceeded == false)
        {
            printf("ERROR: failed to create the compiler process, please debug.\n");
        }
        else
        {
            WaitForSingleObject(CompilerProcessProcessInfo.hProcess, INFINITE);
            CloseHandle(CompilerProcessProcessInfo.hProcess);
            CloseHandle(CompilerProcessProcessInfo.hThread);
        }
    }
    return 0;
}
