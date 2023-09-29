#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>

#include "..\platform\base_types.h"
#include "..\platform\assertions.h"
#include "..\platform\basic_defines.h"
#include "..\platform\files_and_folders\files_and_folders.h"

#include "..\math\scalar_conversions.cpp"

#include "..\platform\console\console.cpp"
#include "..\platform\files_and_folders\files_and_folders.cpp"
#include "..\platform\processes_and_threads\processes.cpp"

#include "build_helpers.cpp"

void DisplayHelp()
{
    printf("INFO: Available build targets:\n");
    printf("          build help\n");
    printf("          build clean\n");
    printf("          build build\n");
    printf("          build basic_app\n");
    printf("          build handmade_hero\n");
    printf("          build directx_demo [debug, release]\n");
    printf("          build imgui_demo [opengl2, dx11]\n");
    printf("          build ray_tracer [optimized, non_optimized] [1_lane, 4_lanes, 8_lanes]\n");
    printf("          build lint [job_per_directory]\n");
    printf("          build x86_kernel\n");
    printf("          build compilation_tests\n");
    printf("          build fat12_tests\n");
}

int main(int argc, char **argv)
{
    char OutputDirectoryPath[1024];
    ZeroMemory(OutputDirectoryPath, ArrayCount(OutputDirectoryPath));
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    char RootDirectoryPath[1024];
    ZeroMemory(RootDirectoryPath, ArrayCount(RootDirectoryPath));
    StringCchCatA(RootDirectoryPath, ArrayCount(RootDirectoryPath), OutputDirectoryPath);
    StringCchCatA(RootDirectoryPath, ArrayCount(RootDirectoryPath), "\\..");

    InitializeConsole();

    if (argc < 2)
    {
        ConsolePrintColored("ERROR: No build target.\n", FOREGROUND_RED);
        DisplayHelp();
        return 1;
    }

    if (strcmp(argv[1], "clean") == 0)
    {
        const char *ExtensionsToClean[] =
        {
            "obj", "pdb", "log", "ilk", "sln", "bmp", "txt",
            "ini", "dll", "exp", "lib", "map", "hmi", "cso",
            "lock", "exe", "img", "h"
        };

        for (u32 ExtensionIndex = 0; ExtensionIndex < ArrayCount(ExtensionsToClean); ExtensionIndex++)
        {
            u32 BuildSuccess = CleanExtensionFromDirectory(ExtensionsToClean[ExtensionIndex], OutputDirectoryPath);
            if (!BuildSuccess)
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
        b32 BuildSuccess = FALSE;

        char CompilerFlags[1024];
        ZeroMemory(CompilerFlags, ArrayCount(CompilerFlags));

        char LinkerFlags[1024];
        ZeroMemory(LinkerFlags, ArrayCount(LinkerFlags));

        char SourcesString[1024];
        ZeroMemory(SourcesString, ArrayCount(SourcesString));

        char OutputBinaryPath[1024];
        ZeroMemory(OutputBinaryPath, ArrayCount(OutputBinaryPath));

        if (strcmp(argv[1], "build") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /FC /Oi /GR- /EHa- ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\build\\build.cpp");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\build.temp.exe");

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "basic_app") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /Z7 /FC /Oi /Od /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\basic_app\\main.cpp");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:windows /incremental:no /opt:ref user32.lib gdi32.lib winmm.lib ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\basic_app.exe");

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "ray_tracer") == 0)
        {
            if (argc < 4)
            {
                ConsolePrintColored("ERROR: invalid number of arguments for build ray_tracer ...\n", FOREGROUND_RED);
                DisplayHelp();
                return 1;
            }

            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);

            if (strcmp(argv[2], "optimized") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/O2 ");
            }
            else if (strcmp(argv[2], "non_optimized") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/Od ");
            }
            else
            {
                ConsoleSwitchColor(FOREGROUND_RED);
                printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", argv[2]);
                ConsoleResetColor();
                DisplayHelp();
                return 1;
            }

            if (strcmp(argv[3], "1_lane") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DSIMD_NUMBEROF_LANES=1 ");
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\ray_tracer_1.exe");
            }
            else if (strcmp(argv[3], "4_lanes") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DSIMD_NUMBEROF_LANES=4 ");
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\ray_tracer_4.exe");
            }
            else if (strcmp(argv[3], "8_lanes") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DSIMD_NUMBEROF_LANES=8 ");
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\ray_tracer_8.exe");
            }
            else
            {
                ConsoleSwitchColor(FOREGROUND_RED);
                printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", argv[3]);
                ConsoleResetColor();
                DisplayHelp();
                return 1;
            }

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\ray_tracer\\main.cpp");


            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib gdi32.lib ");

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "imgui_demo") == 0)
        {
            if (argc < 3)
            {
                ConsolePrintColored("ERROR: invalid number of arguments for build imgui_demo ...\n", FOREGROUND_RED);
                DisplayHelp();
                return 1;
            }

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\third_party\\imgui\\imgui*.cpp ");

            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /Zi /MD /utf-8 /DUNICODE /D_UNICODE /DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "user32.lib Gdi32.lib dwmapi.lib ");

            if (strcmp(argv[2], "opengl2") == 0)
            {
                StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
                StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\imgui_demo\\main_opengl2.cpp ");

                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\imgui_demo_opengl2.exe");

                StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "opengl32.lib ");
            }
            else if (strcmp(argv[2], "dx11") == 0)
            {
                StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
                StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\imgui_demo\\main_dx11.cpp ");

                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\imgui_demo_dx11.exe");

                StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "d3d11.lib d3dcompiler.lib ");
            }
            else
            {
                ConsoleSwitchColor(FOREGROUND_RED);
                printf("ERROR: invalid argument \"%s\" for build imgui_demo ...\n", argv[2]);
                ConsoleResetColor();
                DisplayHelp();
                return 1;
            }

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "handmade_hero") == 0)
        {
            char SharedCompilerFlags[1024];
            ZeroMemory(SharedCompilerFlags, ArrayCount(SharedCompilerFlags));

            StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 ");
            StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/DHANDMADE_WIN32=1 /DHANDMADE_SLOW=1 /DHANDMADE_INTERNAL=1 /DENABLE_ASSERTIONS ");
            StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/nologo /Zi /FC /Od /Oi /GR- /EHa- /Gm- /MTd ");

            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), SharedCompilerFlags);
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/LD /Fmgame.map ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\handmade_hero\\game.cpp ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\game.dll");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/incremental:no ");
            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/EXPORT:GameGetSoundSamples /EXPORT:GameUpdateAndRender ");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/PDB:game_");
            char RandomString[5];
            ZeroMemory(RandomString, ArrayCount(RandomString));
            u32 RandomNumber;
            rand_s(&RandomNumber);
            RandomNumber = (u32)((f32)RandomNumber / (f32)UINT_MAX * 9999.0f);
            StringCchPrintfA(RandomString, ArrayCount(RandomString), "%d", RandomNumber);
            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), RandomString);
            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), ".pdb ");

            char LockFilePath[MAX_PATH];
            ZeroMemory(LockFilePath, ArrayCount(LockFilePath));
            StringCchCatA(LockFilePath, ArrayCount(LockFilePath), "compilation.lock");

            CreateFileA(LockFilePath, 0, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
            DeleteFileA(LockFilePath);

            ZeroMemory(CompilerFlags, ArrayCount(CompilerFlags));
            ZeroMemory(SourcesString, ArrayCount(SourcesString));
            ZeroMemory(OutputBinaryPath, ArrayCount(OutputBinaryPath));
            ZeroMemory(LinkerFlags, ArrayCount(LinkerFlags));

            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), SharedCompilerFlags);
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/Fmwin32_platform.map ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\handmade_hero\\win32_platform.cpp ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\win32_platform.exe");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/incremental:no /subsystem:windows /opt:ref ");
            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "user32.lib gdi32.lib winmm.lib ");

            BuildSuccess = BuildSuccess && CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "directx_demo") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /FC /Oi /GR- /EHa- /Zi /MD ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");

            if (argc < 3)
            {
                ConsolePrintColored("ERROR: invalid number of arguments for build directx_demo.\n", FOREGROUND_RED);
                DisplayHelp();
                return 1;
            }
            else if(strcmp(argv[2], "debug") == 0)
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/D_DEBUG=1 ");
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\directx_demo_debug.exe");
            }
            else if(strcmp(argv[2], "release") == 0)
            {
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
                StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\directx_demo.exe");
            }
            else
            {
                ConsoleSwitchColor(FOREGROUND_RED);
                printf("ERROR: invalid argument \"%s\" for build directx_demo ...\n", argv[2]);
                ConsoleResetColor();
                DisplayHelp();
                return 1;
            }

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\directx_demo\\main.cpp");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:windows /incremental:no /opt:ref ");
            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "user32.lib winmm.lib d3d11.lib dxgi.lib d3dcompiler.lib ");

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);

            ZeroMemory(CompilerFlags, ArrayCount(CompilerFlags));
            ZeroMemory(SourcesString, ArrayCount(SourcesString));
            ZeroMemory(OutputBinaryPath, ArrayCount(OutputBinaryPath));

            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/T vs_4_0 /Od /Zi /E SimpleVertexShader /Fh vertex_shader.h /Vn GlobalVertexShader");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\directx_demo\\vertex_shader.hlsl");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\vertex_shader.cso");

            BuildSuccess = BuildSuccess && CompileShader(CompilerFlags, SourcesString, OutputBinaryPath);

            ZeroMemory(CompilerFlags, ArrayCount(CompilerFlags));
            ZeroMemory(SourcesString, ArrayCount(SourcesString));
            ZeroMemory(OutputBinaryPath, ArrayCount(OutputBinaryPath));

            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/T ps_4_0 /Od /Zi /E SimplePixelShader /Fh pixel_shader.h /Vn GlobalPixelShader");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\windows_apps\\directx_demo\\pixel_shader.hlsl");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\pixel_shader.cso");

            BuildSuccess = BuildSuccess && CompileShader(CompilerFlags, SourcesString, OutputBinaryPath);
        }
        else if (strcmp(argv[1], "lint") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            if
            (
                (argc >= 3) &&
                (strcmp(argv[2], "job_per_directory") == 0)
            )
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DJOB_PER_DIRECTORY ");
            }
            else
            {
                StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DJOB_PER_FILE ");
            }

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\build\\lint.cpp");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:console /incremental:no /opt:ref Shlwapi.lib ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\lint.exe");

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "x86_kernel") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "-f bin ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\low_level\\x86_kernel\\bootloader.s");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\x86_bootloader.img");

            BuildSuccess = CompileAssembly(CompilerFlags, SourcesString, OutputBinaryPath);

            ZeroMemory(CompilerFlags, ArrayCount(CompilerFlags));
            ZeroMemory(SourcesString, ArrayCount(SourcesString));
            ZeroMemory(OutputBinaryPath, ArrayCount(OutputBinaryPath));

            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "-f bin ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\low_level\\x86_kernel\\kernel.s");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\x86_kernel.img");

            BuildSuccess = BuildSuccess && CompileAssembly(CompilerFlags, SourcesString, OutputBinaryPath);

            ZeroMemory(OutputBinaryPath, ArrayCount(OutputBinaryPath));
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\floppy.img");

            BuildSuccess = BuildSuccess && CreateEmptyFile(OutputBinaryPath, 512 * 2880, 0);

            // mkfs.fat -F 12 -n "NBOS" floppy.img

            char SourceBinaryFilePath[1024];
            char DestinationBinaryFilePath[1024];
            ZeroMemory(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath));
            ZeroMemory(DestinationBinaryFilePath, ArrayCount(DestinationBinaryFilePath));

            StringCchCatA(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath), OutputDirectoryPath);
            StringCchCatA(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath), "\\x86_bootloader.img");

            StringCchCatA(DestinationBinaryFilePath, ArrayCount(DestinationBinaryFilePath), OutputDirectoryPath);
            StringCchCatA(DestinationBinaryFilePath, ArrayCount(DestinationBinaryFilePath), "\\floppy.img");

            BuildSuccess = BuildSuccess && WriteBinaryFileOverAnother(SourceBinaryFilePath, 0, DestinationBinaryFilePath);
        }
        else if (strcmp(argv[1], "compilation_tests") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127 ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\build\\compilation_tests.cpp");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\compilation_tests.exe");

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else if (strcmp(argv[1], "fat12_tests") == 0)
        {
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except- ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127 ");
            StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS ");

            StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
            StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\low_level\\fat12\\test.cpp");

            StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib ");

            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
            StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\fat12_tests.exe");

            BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
        }
        else
        {
            ConsoleSwitchColor(FOREGROUND_RED);
            printf("ERROR: invalid build target \"%s\".\n", argv[1]);
            ConsoleResetColor();
            DisplayHelp();
            return 1;
        }

        if (!BuildSuccess)
        {
            return 1;
        }
    }

    return 0;
}