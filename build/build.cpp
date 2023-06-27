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

void CleanupBinFolder(const char *BinDirectoryPath)
{
    printf("\n====== Cleanup ======\n");

    const char *ExtensionsToClean[] = 
    {
        "obj", "pdb", "exe", "log", "ilk", "sln"
    };

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
}

void InvokeCompiler
(
    const char *CompilerFlags,
    const char *MainTranslationUnitPath, 
    const char *LinkerFlags,
    const char *OutputBinaryPath
)
{
    printf("\n====== Compiler output ======\n");

    STARTUPINFO CompilerProcessStartupInfo = {};
    CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
    PROCESS_INFORMATION CompilerProcessProcessInfo = {};

    char CompilerCommand[1024];
    ZeroMemory(CompilerCommand, 1024);
    StringCchCatA(CompilerCommand, 1024, "cl.exe ");
    StringCchCatA(CompilerCommand, 1024, CompilerFlags);
    StringCchCatA(CompilerCommand, 1024, " ");
    StringCchCatA(CompilerCommand, 1024, MainTranslationUnitPath);
    StringCchCatA(CompilerCommand, 1024, " /Fe:\"");
    StringCchCatA(CompilerCommand, 1024, OutputBinaryPath);
    StringCchCatA(CompilerCommand, 1024, "\"");
    StringCchCatA(CompilerCommand, 1024, " /link ");
    StringCchCatA(CompilerCommand, 1024, LinkerFlags);

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

int main(int argc, char **argv)
{
    const char *RootDirectoryPath = "F:\\thing";

    char BinDirectoryPath[MAX_PATH];
    ZeroMemory(BinDirectoryPath, ArrayLength(BinDirectoryPath));
    StringCchCatA(BinDirectoryPath, ArrayLength(BinDirectoryPath), RootDirectoryPath);
    StringCchCatA(BinDirectoryPath, ArrayLength(BinDirectoryPath), "\\bin");

    if (argc >= 2)
    {
        if (strcmp(argv[1], "clean") == 0)
        {
            CleanupBinFolder(BinDirectoryPath);
        }
        else 
        {
            CleanupBinFolder(BinDirectoryPath);

            char CompilerFlags[512];
            ZeroMemory(CompilerFlags, 512);
            StringCchCatA(CompilerFlags, 512, "-nologo -Zi -Zo -FC -Oi -GR- -EHa- -Gm- -MTd -fp:fast -fp:except- ");
            if ((argc == 3) && (strcmp(argv[2], "optimized") == 0))
            {
                StringCchCatA(CompilerFlags, 512, "-O2 ");
            }
            else
            {
                StringCchCatA(CompilerFlags, 512, "-Od ");
            }
            StringCchCatA(CompilerFlags, 512, "-W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4996 -wd4018 ");

            char LinkerFlags[512];
            ZeroMemory(LinkerFlags, 512);
            StringCchCatA(LinkerFlags, 512, "/incremental:no /opt:ref user32.lib gdi32.lib winmm.lib ");

            char SourceTranslationUnitPath[MAX_PATH];
            ZeroMemory(SourceTranslationUnitPath, ArrayLength(SourceTranslationUnitPath));
            StringCchCatA(SourceTranslationUnitPath, ArrayLength(SourceTranslationUnitPath), RootDirectoryPath);

            char OutputBinaryPath[MAX_PATH];
            ZeroMemory(OutputBinaryPath, ArrayLength(OutputBinaryPath));
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), BinDirectoryPath);

            if (strcmp(argv[1], "simulator") == 0)
            {
                StringCchCatA(SourceTranslationUnitPath, ArrayLength(SourceTranslationUnitPath), "\\simulator\\main.cpp");
                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\simulator.exe");
                StringCchCatA(LinkerFlags, 512, "/subsystem:windows ");
            }
            else if (strcmp(argv[1], "ray_tracer") == 0)
            {
                StringCchCatA(SourceTranslationUnitPath, ArrayLength(SourceTranslationUnitPath), "\\ray_tracer\\main.cpp");
                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\ray_tracer.exe");
                StringCchCatA(CompilerFlags, 512, "-DINTERNAL -DSLOW -D_CRT_SECURE_NO_WARNINGS ");
                StringCchCatA(LinkerFlags, 512, "/subsystem:console ");
            }
            else
            {
                printf("ERROR: invalid build target \"%s\".\n", argv[1]);
                return 1;
            }

            InvokeCompiler(CompilerFlags, SourceTranslationUnitPath, LinkerFlags, OutputBinaryPath);
        }
    }
    else
    {
        printf("ERROR: No build target.\n");
        printf("INFO: Available build targets:\n");
        printf("INFO:       build simulator\n");
        printf("INFO:       build ray_tracer\n");
        printf("INFO:       build clean --> cleans up the bin\\ directory\n");
        return 1;
    }

    return 0;
}
