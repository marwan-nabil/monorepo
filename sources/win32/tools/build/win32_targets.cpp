#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\shared\base_types.h"
#include "sources\win32\shared\basic_defines.h"
#include "sources\win32\shared\console\console.h"
#include "build.h"
#include "build_helpers.h"
#include "compiler_helpers.h"

b32 BuildCompilationTests(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tests\\compilation_tests\\compilation_tests.cpp");

    AddCompilerFlags(BuildContext, "/nologo /FC /O2 /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetLinkerOutputBinary(BuildContext, "\\compilation_tests.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}


b32 BuildDirectxDemo(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\directx\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /FC /Oi /O2 /GR- /EHa- /Zi /MD");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/subsystem:windows /incremental:no /opt:ref");
    AddLinkerFlags(BuildContext, "user32.lib winmm.lib d3d11.lib dxgi.lib D3DCompiler.lib");

    SetLinkerOutputBinary(BuildContext, "\\directx_demo.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\directx\\vertex_shader.hlsl");
    AddCompilerFlags
    (
        BuildContext,
        "/T vs_4_0 /Od /Zi /E SimpleVertexShader /Vn GlobalVertexShader"
    );
    SetCompilerOutputObject(BuildContext, "\\vertex_shader.cso");
    BuildSuccess = CompileShader(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\directx\\pixel_shader.hlsl");
    AddCompilerFlags
    (
        BuildContext,
        "/T ps_4_0 /Od /Zi /E SimplePixelShader /Vn GlobalPixelShader"
    );
    SetCompilerOutputObject(BuildContext, "\\pixel_shader.cso");
    BuildSuccess = CompileShader(BuildContext);
    return BuildSuccess;
}

b32 BuildFat12Tests(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tests\\fat12_tests\\test.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetLinkerOutputBinary(BuildContext, "\\fat12_tests.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}

b32 BuildFetchData(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tools\\fetch_data\\fetch_data.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib winhttp.lib");

    SetLinkerOutputBinary(BuildContext, "\\fetch_data.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}

b32 BuildHandmadeHero(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\handmade_hero\\game.cpp");

    char SharedCompilerFlags[1024] = {};
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/nologo /Zi /FC /Od /Oi /GR- /EHa- /Gm- /MTd ");
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 ");
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/DHANDMADE_WIN32=1 /DHANDMADE_SLOW=1 /DHANDMADE_INTERNAL=1 /DENABLE_ASSERTIONS ");

    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/LD /Fmgame.map");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/incremental:no");
    AddLinkerFlags(BuildContext, "/EXPORT:GameGetSoundSamples /EXPORT:GameUpdateAndRender");

    char PdbLinkerFlag[1024] = {};
    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), "/PDB:game_");

    char RandomString[5] = {};
    u32 RandomNumber;
    rand_s(&RandomNumber);
    RandomNumber = (u32)((f32)RandomNumber / (f32)UINT_MAX * 9999.0f);
    StringCchPrintfA(RandomString, ArrayCount(RandomString), "%d", RandomNumber);

    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), RandomString);
    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), ".pdb");

    AddLinkerFlags(BuildContext, PdbLinkerFlag);

    SetLinkerOutputBinary(BuildContext, "\\game.dll");

    char LockFilePath[MAX_PATH] = {};
    StringCchCatA(LockFilePath, ArrayCount(LockFilePath), "compilation.lock");

    CreateFileA(LockFilePath, 0, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    DeleteFileA(LockFilePath);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\handmade_hero\\win32_platform.cpp");

    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/Fmwin32_platform.map");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/incremental:no /subsystem:windows /opt:ref");
    AddLinkerFlags(BuildContext, "user32.lib gdi32.lib winmm.lib");

    SetLinkerOutputBinary(BuildContext, "\\win32_platform.exe");

    BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}

b32 BuildImguiDemo(build_context *BuildContext)
{
    if (BuildContext->EnvironmentInfo.argc < 3)
    {
        ConsolePrintColored
        (
            "ERROR: invalid number of arguments for build imgui_demo ...\n",
            BuildContext->EnvironmentInfo.ConsoleContext,
            FOREGROUND_RED
        );
        DisplayHelp();
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\imgui\\imgui*.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Zi /MD /utf-8");
    AddCompilerFlags(BuildContext, "/DUNICODE /D_UNICODE /DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "user32.lib Gdi32.lib dwmapi.lib");

    if (strcmp(BuildContext->EnvironmentInfo.argv[2], "opengl2") == 0)
    {
        AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\imgui\\main_opengl2.cpp");
        AddLinkerFlags(BuildContext, "opengl32.lib");
        SetLinkerOutputBinary(BuildContext, "\\imgui_demo_opengl2.exe");
    }
    else if (strcmp(BuildContext->EnvironmentInfo.argv[2], "dx11") == 0)
    {
        AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\imgui\\main_dx11.cpp");
        AddLinkerFlags(BuildContext, "d3d11.lib d3dcompiler.lib");
        SetLinkerOutputBinary(BuildContext, "\\imgui_demo_dx11.exe");
    }
    else
    {
        ConsoleSwitchColor(BuildContext->EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
        printf("ERROR: invalid argument \"%s\" for build imgui_demo ...\n", BuildContext->EnvironmentInfo.argv[2]);
        ConsoleResetColor(BuildContext->EnvironmentInfo.ConsoleContext);
        DisplayHelp();
        return FALSE;
    }

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}

b32 BuildLint(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tools\\lint\\lint.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    if
    (
        (BuildContext->EnvironmentInfo.argc >= 3) &&
        (strcmp(BuildContext->EnvironmentInfo.argv[2], "job_per_directory") == 0)
    )
    {
        AddCompilerFlags(BuildContext, "/DJOB_PER_DIRECTORY");
    }
    else
    {
        AddCompilerFlags(BuildContext, "/DJOB_PER_FILE");
    }

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref Shlwapi.lib");

    SetLinkerOutputBinary(BuildContext, "\\lint.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}

b32 BuildRayTracer(build_context *BuildContext)
{
    if (BuildContext->EnvironmentInfo.argc < 3)
    {
        ConsolePrintColored("ERROR: invalid number of arguments for build ray_tracer ...\n", BuildContext->EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
        DisplayHelp();
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\ray_tracer\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /O2 /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib gdi32.lib");

    if (strcmp(BuildContext->EnvironmentInfo.argv[2], "1_lane") == 0)
    {
        AddCompilerFlags(BuildContext, "/DSIMD_NUMBEROF_LANES=1");
        SetLinkerOutputBinary(BuildContext, "\\ray_tracer_1.exe");
    }
    else if (strcmp(BuildContext->EnvironmentInfo.argv[2], "4_lanes") == 0)
    {
        AddCompilerFlags(BuildContext, "/DSIMD_NUMBEROF_LANES=4");
        SetLinkerOutputBinary(BuildContext, "\\ray_tracer_4.exe");
    }
    else if (strcmp(BuildContext->EnvironmentInfo.argv[2], "8_lanes") == 0)
    {
        AddCompilerFlags(BuildContext, "/DSIMD_NUMBEROF_LANES=8");
        SetLinkerOutputBinary(BuildContext, "\\ray_tracer_8.exe");
    }
    else
    {
        ConsoleSwitchColor(BuildContext->EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
        printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", BuildContext->EnvironmentInfo.argv[3]);
        ConsoleResetColor(BuildContext->EnvironmentInfo.ConsoleContext);
        DisplayHelp();
        return FALSE;
    }

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}

b32 BuildSimulator(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\simulator\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/subsystem:windows /incremental:no /opt:ref user32.lib gdi32.lib winmm.lib");

    SetLinkerOutputBinary(BuildContext, "\\simulator.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}