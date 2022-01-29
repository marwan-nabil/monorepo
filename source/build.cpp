#pragma comment(lib, "Shell32.lib")

#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>

#include "platform_types.h"
#include "utility_macros.h"

#include "AsciiStrings.cpp"
#include "FileOperations.cpp"

int main(int argc, char **argv)
{
    const char *SourceDirectoryAbsolutePath = "W:\\thing\\build";
    bool32 Error = false;

    if (StringLength(SourceDirectoryAbsolutePath) > (MAX_PATH - 3))
    {
        printf("Path that will be used to build is too long!");
        Error = true;
    }

    if (Error == false)
    {
        const char *ExtensionsOfFilesToBeCleaned[4] = 
        {
            "obj", "pdb", "exe", "log"
        };

        for (u32 Index = 0; Index < ArrayLength(ExtensionsOfFilesToBeCleaned); Index++)
        {
            DeleteFilesWithExtention(ExtensionsOfFilesToBeCleaned[Index], SourceDirectoryAbsolutePath);
        }

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
    }

    return 0;
}
