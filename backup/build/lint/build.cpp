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

b32 BuildLint(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\build\\lint.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    if
    (
        (BuildContext->argc >= 3) &&
        (strcmp(BuildContext->argv[2], "job_per_directory") == 0)
    )
    {
        AddCompilerFlags(BuildContext, "/DJOB_PER_DIRECTORY");
    }
    else
    {
        AddCompilerFlags(BuildContext, "/DJOB_PER_FILE");
    }


    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref Shlwapi.lib");

    SetOuputBinaryPath(BuildContext, "\\lint.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}