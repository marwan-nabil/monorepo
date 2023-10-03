#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>

#include "..\platform\base_types.h"
#include "..\platform\basic_defines.h"
#include "build.h"

b32 BuildBuild(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\build\\main.cpp");

    for (u32 TargetIndex = 0; TargetIndex < ArrayCount(BuildTargetMappings); TargetIndex++)
    {
        AddSourceFile(BuildContext, BuildTargetMappings[TargetIndex].BuildFilePath);
    }

    AddCompilerFlags(BuildContext, "/nologo /Oi /FC /Od /GR- /EHa-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\build.temp.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}