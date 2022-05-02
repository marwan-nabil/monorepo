#pragma comment(lib, "Shell32.lib")

#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>

#include "platform_types.h"
#include "utility_macros.h"

#include "ascii_strings.cpp"
#include "file_operations.cpp"

int main(int argc, char **argv)
{
    bool32 Error = false;

    const char *RootDirectoryPath = "W:\\thing";
    Error = ValidateStringLength
    (
        RootDirectoryPath, 
        MAX_PATH, 
        "ERROR: main(): RootDirectoryPath is over MAX_PATH!"
    );

    if (Error == false)
    {
        const char *ExtensionsToClean[4] = 
        {
            "obj", "pdb", "exe", "log"
        };

        if ((StringLength(RootDirectoryPath) + StringLength("\\source")) >= MAX_PATH)
        {
            printf("ERROR: main(): RootDirectoryPath + \"\\source\" is over MAX_PATH!\n");
            Error = true;
        }
        if ((StringLength(RootDirectoryPath) + StringLength("\\binaries")) >= MAX_PATH)
        {
            printf("ERROR: main(): RootDirectoryPath + \"\\binaries\" is over MAX_PATH!\n");
            Error = true;
        }

        if (Error == false)
        {
            char SourceDirectoryPath[MAX_PATH];
            ZeroMemory(SourceDirectoryPath, ArrayLength(SourceDirectoryPath));
            AppendString(SourceDirectoryPath, ArrayLength(SourceDirectoryPath), RootDirectoryPath);
            AppendString(SourceDirectoryPath, ArrayLength(SourceDirectoryPath), "\\source");

            char BinariesDirectoryPath[MAX_PATH];
            ZeroMemory(BinariesDirectoryPath, ArrayLength(BinariesDirectoryPath));
            AppendString(BinariesDirectoryPath, ArrayLength(BinariesDirectoryPath), RootDirectoryPath);
            AppendString(BinariesDirectoryPath, ArrayLength(BinariesDirectoryPath), "\\binaries");

            for (u32 Index = 0; Index < ArrayLength(ExtensionsToClean); Index++)
            {
                DeleteFilesWithExtentionFromDirectory(ExtensionsToClean[Index], BinariesDirectoryPath);
            }

            STARTUPINFO CompilerProcessStartupInfo = {};
            CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
            PROCESS_INFORMATION CompilerProcessProcessInfo = {};

            if ((StringLength(SourceDirectoryPath) + StringLength("\\main.cpp")) >= MAX_PATH)
            {
                printf("ERROR: main(): SourceDirectoryPath + \"\\main.cpp\" is over MAX_PATH!\n");
                Error = true;
            }

            if (Error == false)
            {
                char MainTranslationUnitPath[MAX_PATH];
                ZeroMemory(MainTranslationUnitPath, ArrayLength(MainTranslationUnitPath));
                AppendString(MainTranslationUnitPath, ArrayLength(MainTranslationUnitPath), SourceDirectoryPath);
                AppendString(MainTranslationUnitPath, ArrayLength(MainTranslationUnitPath), "\\main.cpp");

                char CompilerCommand[1024];
                ZeroMemory(CompilerCommand, 1024);

                AppendString(CompilerCommand, 1024, "cl.exe");
                AppendString(CompilerCommand, 1024, " -nologo -Zi -FC -Od -Oi -GR- -EHa- -Gm- -MTd");
                AppendString(CompilerCommand, 1024, " -W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4996");
                AppendString(CompilerCommand, 1024, " ");
                AppendString(CompilerCommand, 1024, MainTranslationUnitPath);
                AppendString(CompilerCommand, 1024, " /link /incremental:no /subsystem:windows /opt:ref");
                AppendString(CompilerCommand, 1024, " -Fe");
                AppendString(CompilerCommand, 1024, BinariesDirectoryPath);
                AppendString(CompilerCommand, 1024, "\\main.exe");

                BOOL CreateSucceeded = CreateProcess
                (
                    NULL,           // No module name (use command line)
                    CompilerCommand,// Command line
                    NULL,           // Process handle not inheritable
                    NULL,           // Thread handle not inheritable
                    FALSE,          // Set handle inheritance to FALSE
                    0,              // No creation flags
                    NULL,           // Use parent's environment block
                    NULL,           // Use parent's starting directory 
                    &CompilerProcessStartupInfo,            // Pointer to STARTUPINFO structure
                    &CompilerProcessProcessInfo
                );

                if (CreateSucceeded == false)
                {
                    printf("ERROR: failed to create compiler process");
                    Error = true;
                }

                if (Error == false)
                {
                    WaitForSingleObject(CompilerProcessProcessInfo.hProcess, INFINITE);
                    CloseHandle(CompilerProcessProcessInfo.hProcess);
                    CloseHandle(CompilerProcessProcessInfo.hThread);
                }
            }
        }
    }

    return 0;
}
