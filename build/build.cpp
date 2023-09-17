#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
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
    printf("          build basic_app\n");
    printf("          build handmade_hero\n");
    printf("          build directx_demo\n");
    printf("          build imgui_demo [opengl2, dx11]\n");
    printf("          build ray_tracer [optimized, non_optimized] [1_lane, 4_lanes, 8_lanes]\n");
}

u32 CleanExtensionFromDirectory(const char *ExtensionToClean, const char *DirectoryPath)
{
    char FilesWildcard[MAX_PATH];
    ZeroMemory(FilesWildcard, ArrayLength(FilesWildcard));
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
    ZeroMemory(CompilerCommand, ArrayLength(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), "fxc.exe ");
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), " /Fo \"");
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), SourcesString);

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
    ZeroMemory(CompilerCommand, ArrayLength(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), "cl.exe ");
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), SourcesString);
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), " /Fe:\"");
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayLength(CompilerCommand), "/link ");
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
            "obj", "pdb", "exe", "log", "ilk", "sln", "bmp",
            "txt", "ini", "dll", "exp", "lib", "map", "hmi",
            "cso", "lock"
        };

        for (u32 ExtensionIndex = 0; ExtensionIndex < ArrayLength(ExtensionsToClean); ExtensionIndex++)
        {
            u32 CompilationSuccess = CleanExtensionFromDirectory(ExtensionsToClean[ExtensionIndex], OutputDirectoryPath);
            if (CompilationSuccess)
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
        b32 CompilationSuccess = FALSE;

        char CompilerFlags[1024];
        ZeroMemory(CompilerFlags, ArrayLength(CompilerFlags));

        char LinkerFlags[1024];
        ZeroMemory(LinkerFlags, ArrayLength(LinkerFlags));

        char SourcesString[1024];
        ZeroMemory(SourcesString, ArrayLength(SourcesString));

        char OutputBinaryPath[1024];
        ZeroMemory(OutputBinaryPath, ArrayLength(OutputBinaryPath));

        if (strcmp(argv[1], "build") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/nologo /FC /Oi /GR- /EHa- ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S ");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\build\\build.cpp");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib ");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\build.temp.exe");

            CompilationSuccess = InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "test") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\tests\\test.cpp");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib ");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\test.exe");

            CompilationSuccess = InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "basic_app") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/nologo /Z7 /FC /Oi /Od /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\basic_app\\main.cpp");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:windows /incremental:no /opt:ref user32.lib gdi32.lib winmm.lib ");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\basic_app.exe");

            CompilationSuccess = InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "ray_tracer") == 0)
        {
            if (argc < 4)
            {
                printf("ERROR: invalid number of arguments for build ray_tracer ...\n");
                DisplayHelp();
                return 1;
            }

            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            if (strcmp(argv[2], "optimized") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/O2 ");
            }
            else if (strcmp(argv[2], "non_optimized") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/Od ");
            }
            else
            {
                printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", argv[2]);
                DisplayHelp();
                return 1;
            }

            if (strcmp(argv[3], "1_lane") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/DSIMD_NUMBEROF_LANES=1 ");
            }
            else if (strcmp(argv[3], "4_lanes") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/DSIMD_NUMBEROF_LANES=4 ");
            }
            else if (strcmp(argv[3], "8_lanes") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/DSIMD_NUMBEROF_LANES=8 ");
            }
            else
            {
                printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", argv[3]);
                DisplayHelp();
                return 1;
            }

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\ray_tracer\\main.cpp");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\ray_tracer.exe");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib gdi32.lib ");

            CompilationSuccess = InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "imgui_demo") == 0)
        {
            if (argc < 3)
            {
                printf("ERROR: invalid number of arguments for build imgui_demo ...\n");
                DisplayHelp();
                return 1;
            }

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\libraries\\imgui\\imgui*.cpp ");

            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/nologo /Zi /MD /utf-8 /DUNICODE /D_UNICODE /DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "user32.lib Gdi32.lib dwmapi.lib ");

            if (strcmp(argv[2], "opengl2") == 0)
            {
                StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
                StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\imgui_demo\\main_opengl2.cpp ");

                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
                StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\imgui_demo_opengl2.exe");

                StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "opengl32.lib ");
            }
            else if (strcmp(argv[2], "dx11") == 0)
            {
                StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
                StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\imgui_demo\\main_dx11.cpp ");

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

            CompilationSuccess = InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "handmade_hero") == 0)
        {
            char SharedCompilerFlags[1024];
            ZeroMemory(SharedCompilerFlags, ArrayLength(SharedCompilerFlags));

            StringCchCatA(SharedCompilerFlags, ArrayLength(SharedCompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 ");
            StringCchCatA(SharedCompilerFlags, ArrayLength(SharedCompilerFlags), "/DHANDMADE_WIN32=1 /DHANDMADE_SLOW=1 /DHANDMADE_INTERNAL=1 ");
            StringCchCatA(SharedCompilerFlags, ArrayLength(SharedCompilerFlags), "/nologo /Zi /FC /Od /Oi /GR- /EHa- /Gm- /MTd ");

            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), SharedCompilerFlags);
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/LD /Fmhandmade.map ");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\handmade_hero\\game.cpp ");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\handmade.dll");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/incremental:no ");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/EXPORT:GameGetSoundSamples /EXPORT:GameUpdateAndRender ");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/PDB:handmade_");
            char RandomString[5];
            ZeroMemory(RandomString, ArrayLength(RandomString));
            u32 RandomNumber;
            rand_s(&RandomNumber);
            RandomNumber = (u32)((f32)RandomNumber / (f32)UINT_MAX * 9999.0f);
            StringCchPrintfA(RandomString, ArrayLength(RandomString), "%d", RandomNumber);
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), RandomString);
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), ".pdb ");

            char LockFilePath[MAX_PATH];
            ZeroMemory(LockFilePath, ArrayLength(LockFilePath));
            StringCchCatA(LockFilePath, ArrayLength(LockFilePath), "compilation.lock");

            CreateFileA(LockFilePath, 0, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
            CompilationSuccess = InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
            DeleteFileA(LockFilePath);

            ZeroMemory(CompilerFlags, ArrayLength(CompilerFlags));
            ZeroMemory(SourcesString, ArrayLength(SourcesString));
            ZeroMemory(OutputBinaryPath, ArrayLength(OutputBinaryPath));
            ZeroMemory(LinkerFlags, ArrayLength(LinkerFlags));

            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), SharedCompilerFlags);
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/Fmwin32_handmade.map ");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\handmade_hero\\win32_platform.cpp ");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\win32_platform.exe");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/incremental:no /subsystem:windows /opt:ref ");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "user32.lib gdi32.lib winmm.lib ");

            CompilationSuccess = CompilationSuccess && InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "directx_demo") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/nologo /FC /Oi /GR- /EHa- /Zi /MD ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");
            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\directx_demo\\*.cpp");

            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "/subsystem:windows /incremental:no /opt:ref ");
            StringCchCatA(LinkerFlags, ArrayLength(LinkerFlags), "d3d11.lib gdi32.lib user32.lib ");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\directx_tutorial.exe");

            CompilationSuccess = InvokeCompiler(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);

            ZeroMemory(CompilerFlags, ArrayLength(CompilerFlags));
            ZeroMemory(SourcesString, ArrayLength(SourcesString));
            ZeroMemory(OutputBinaryPath, ArrayLength(OutputBinaryPath));

            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/T ps_4_0_level_9_1");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\directx_demo\\CubePixelShader.hlsl");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\CubePixelShader.cso");

            CompilationSuccess = CompilationSuccess && CompileShader(CompilerFlags, SourcesString, OutputBinaryPath);

            ZeroMemory(CompilerFlags, ArrayLength(CompilerFlags));
            ZeroMemory(SourcesString, ArrayLength(SourcesString));
            ZeroMemory(OutputBinaryPath, ArrayLength(OutputBinaryPath));

            StringCchCatA(CompilerFlags, ArrayLength(CompilerFlags), "/T vs_4_0_level_9_1");

            StringCchCatA(SourcesString, ArrayLength(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayLength(SourcesString), "\\apps\\directx_demo\\CubeVertexShader.hlsl");

            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayLength(OutputBinaryPath), "\\CubeVertexShader.cso");

            CompilationSuccess = CompilationSuccess && CompileShader(CompilerFlags, SourcesString, OutputBinaryPath);
        }
        else
        {
            printf("ERROR: invalid build target \"%s\".\n", argv[1]);
            DisplayHelp();
            return 1;
        }

        if (!CompilationSuccess)
        {
            return 1;
        }
    }

    return 0;
}
