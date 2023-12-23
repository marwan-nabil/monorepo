#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\console\console.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

b32 BuildRayTracer(build_context *BuildContext)
{
    if (BuildContext->EnvironmentInfo.argc < 3)
    {
        ConsolePrintColored("ERROR: invalid number of arguments for build ray_tracer ...\n", BuildContext->EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
        DisplayHelp();
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\ray_tracer\\main.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\ray_tracer\\brdf.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\file_system\\files.cpp");

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