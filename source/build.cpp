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
    const char *SourceDirectoryAbsolutePath = "W:\\thing";
    bool32 Error = false;

    if (StringLength(SourceDirectoryAbsolutePath) > (MAX_PATH - 3))
    {
        printf("ERROR: Path that will be used to build is too long!");
        Error = true;
    }

    if (Error == false)
    {
        const char *ExtensionsToClean[4] = 
        {
            "obj", "pdb", "exe", "log"
        };

        for (u32 Index = 0; Index < ArrayLength(ExtensionsToClean); Index++)
        {
            DeleteFilesWithExtention(ExtensionsToClean[Index], SourceDirectoryAbsolutePath);
        }

        STARTUPINFO CompilerProcessStartupInfo = {};
        CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
        PROCESS_INFORMATION CompilerProcessProcessInfo = {};

        char CompilerCommand[512];
        ZeroMemory(CompilerCommand, 512);

        AppendString(CompilerCommand, 512, "cl.exe");
        AppendString(CompilerCommand, 512, " -nologo -Zi -FC -Od -Oi -GR- -EHa- -Gm- -MTd");
        AppendString(CompilerCommand, 512, " -W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4996");
        AppendString(CompilerCommand, 512, " W:\\thing\\source\\main.cpp");
        AppendString(CompilerCommand, 512, " /link /incremental:no /subsystem:windows /opt:ref");

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

    return 0;
}
