#include <Windows.h>
#include <stdint.h>
#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\string_list.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

b32 BuildLint(build_context *BuildContext)
{
    AddCompilerSourceFileIfRecompilationNeeded(BuildContext, "\\sources\\win32\\tools\\lint\\lint.cpp");
    AddCompilerSourceFileIfRecompilationNeeded(BuildContext, "\\sources\\win32\\libraries\\file_system\\files.cpp");
    AddCompilerSourceFileIfRecompilationNeeded(BuildContext, "\\sources\\win32\\libraries\\strings\\path_handling.cpp");

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

    b32 BuildSuccess = CompileWithMSVC(BuildContext);

    return BuildSuccess;
}

b32 BuildLintOld(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tools\\lint\\lint.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\libraries\\file_system\\files.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\libraries\\strings\\path_handling.cpp");

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

    b32 BuildSuccess = CompileAndLinkWithMSVC(BuildContext);
    return BuildSuccess;
}