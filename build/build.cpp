#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\assertions.h"
#include "..\miscellaneous\basic_defines.h"

void DisplayHelp()
{
    printf("INFO: Available build targets:\n");
    printf("          build help\n");
    printf("          build clean\n");
    printf("          build build\n");
    printf("          build test\n");
    printf("          build imgui_demo [opengl2, dx11]\n");
    printf("          build basic_app\n");
    printf("          build ray_tracer [optimized, non_optimized] [1_lane, 4_lanes, 8_lanes]\n");
}

u32 CleanPerExtension(const char *ExtensionToClean, const char *OutputDirectoryPath)
{
    char FilesWildcard[MAX_PATH];
    ZeroMemory(FilesWildcard, ArrayLength(FilesWildcard));
    StringCchCatA(FilesWildcard, MAX_PATH, OutputDirectoryPath);
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
                StringCchCatA(FoundFilePath, MAX_PATH, OutputDirectoryPath);
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

int main(int argc, char **argv)
{
    char OutputDirectoryPath[1024];
    ZeroMemory(OutputDirectoryPath, ArrayLength(OutputDirectoryPath));
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    char RootDirectoryPath[1024];
    ZeroMemory(RootDirectoryPath, ArrayLength(RootDirectoryPath));
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), OutputDirectoryPath);
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), "\\..");

    if (argc < 2)
    {
        printf("ERROR: No build target.\n");
        DisplayHelp();
        return 1;
    }

    if (strcmp(argv[1], "clean") == 0)
    {
        const char *ExtensionsToClean[] = 
        {
            "obj", "pdb", "exe", "log", "ilk", "sln", "bmp", "txt", "ini"
        };

        for (u32 ExtensionIndex = 0; ExtensionIndex < ArrayLength(ExtensionsToClean); ExtensionIndex++)
        {
            u32 Result = CleanPerExtension(ExtensionsToClean[ExtensionIndex], OutputDirectoryPath);
            if (Result)
            {
                return 2;
            }
        }
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        DisplayHelp();
    }
    else
    {
        char CompilerFlags[1024];
        ZeroMemory(CompilerFlags, ArrayLength(CompilerFlags));

        char LinkerFlags[1024];
        ZeroMemory(LinkerFlags, ArrayLength(LinkerFlags));

        char SourcesString[1024];
        ZeroMemory(SourcesString, ArrayLength(SourcesString));

        char OutputBinaryPath[1024];
        ZeroMemory(OutputBinaryPath, ArrayLength(OutputBinaryPath));

        // ----------------------------------------------------------
        // first argument
        // ----------------------------------------------------------
        if
        (
            (strcmp(argv[1], "build") == 0) ||
            (strcmp(argv[1], "test") == 0) ||
            (strcmp(argv[1], "basic_app") == 0) ||
            (strcmp(argv[1], "ray_tracer") == 0)
        )
        {
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-nologo -Z7 -FC -Oi -GR- -EHa- -MTd -fp:fast -fp:except- ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4996 -wd4018 ");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/incremental:no /opt:ref user32.lib gdi32.lib winmm.lib ");
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
        }

        if (strcmp(argv[1], "build") == 0)
        {
            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\build\\build.cpp");
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\build.temp.exe");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:console ");
        }
        else if (strcmp(argv[1], "test") == 0)
        {
            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\tests\\test.cpp");
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\test.exe");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:console ");
        }
        else if (strcmp(argv[1], "imgui_demo") == 0)
        {
            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\imgui\\imgui*.cpp ");

            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/nologo /Zi /MD /utf-8 /DUNICODE /D_UNICODE ");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "user32.lib Gdi32.lib dwmapi.lib ");
        }
        else if (strcmp(argv[1], "basic_app") == 0)
        {
            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\windows_apps\\basic_app\\main.cpp");
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\basic_app.exe");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-Od ");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:windows ");
        }
        else if (strcmp(argv[1], "ray_tracer") == 0)
        {
            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\ray_tracer\\main.cpp");
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\ray_tracer.exe");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-D_CRT_SECURE_NO_WARNINGS ");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:console ");
        }
        else
        {
            printf("ERROR: invalid build target \"%s\".\n", argv[1]);
            DisplayHelp();
            return 1;
        }

        // ----------------------------------------------------------
        // second argument
        // ----------------------------------------------------------
        if
        (
            (
                (strcmp(argv[1], "ray_tracer") == 0)
                ||
                (strcmp(argv[1], "imgui_demo") == 0)
            )
            &&
            (argc < 3)
        )
        {
            printf("ERROR: invalid number of arguments for build ...\n");
            DisplayHelp();
            return 1;
        }

        if (strcmp(argv[1], "ray_tracer") == 0)
        {
            if (strcmp(argv[2], "optimized") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-O2 ");
            }
            else if (strcmp(argv[2], "non_optimized") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-Od ");
            }
            else
            {
                printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", argv[2]);
                DisplayHelp();
                return 1;
            }
        }
        else if (strcmp(argv[1], "imgui_demo") == 0)
        {
            if (strcmp(argv[2], "opengl2") == 0)
            {
                StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
                StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\windows_apps\\imgui_demo\\main_opengl2.cpp ");

                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\imgui_demo_opengl2.exe");

                StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "opengl32.lib ");
            }
            else if (strcmp(argv[2], "dx11") == 0)
            {
                StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
                StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\windows_apps\\imgui_demo\\main_dx11.cpp ");

                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\imgui_demo_dx11.exe");

                StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "d3d11.lib d3dcompiler.lib ");
            }
            else
            {
                printf("ERROR: invalid argument \"%s\" for build imgui_demo ...\n", argv[2]);
                DisplayHelp();
                return 1;
            }
        }

        // ----------------------------------------------------------
        // third argument
        // ----------------------------------------------------------
        if (strcmp(argv[1], "ray_tracer") == 0)
        {
            if (argc >= 4)
            {
                if (strcmp(argv[3], "1_lane") == 0)
                {
                    StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-DSIMD_NUMBEROF_LANES=1 ");
                }
                else if (strcmp(argv[3], "4_lanes") == 0)
                {
                    StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-DSIMD_NUMBEROF_LANES=4 ");
                }
                else if (strcmp(argv[3], "8_lanes") == 0)
                {
                    StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "-DSIMD_NUMBEROF_LANES=8 ");
                }
                else
                {
                    printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", argv[3]);
                    DisplayHelp();
                    return 1;
                }
            }
            else
            {
                printf("ERROR: invalid number of arguments for build ray_tracer ...\n");
                DisplayHelp();
                return 1;
            }
        }

        STARTUPINFO CompilerProcessStartupInfo = {};
        CompilerProcessStartupInfo.cb = sizeof(CompilerProcessStartupInfo);
        PROCESS_INFORMATION CompilerProcessProcessInfo = {};

        char CompilerCommand[1024];
        ZeroMemory(CompilerCommand, ArrayLength(CompilerCommand));
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), "cl.exe ");
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), CompilerFlags);
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), " ");
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), SourcesString);
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), " /Fe:\"");
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), OutputBinaryPath);
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), "\"");
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), " /link ");
        StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), LinkerFlags);

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
            return 1;
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
                return 1;
            }
        }
    }

    return 0;
}
