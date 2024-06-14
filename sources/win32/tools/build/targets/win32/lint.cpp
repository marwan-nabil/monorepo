#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\string_list.h"
#include "sources\win32\libraries\file_system\files.h"
#include "sources\win32\libraries\cJSON\cJSON.h"

#include "..\..\actions\build_context.h"
#include "..\..\actions\msvc.h"

void MakeBuildGraph(build_context *BuildContext)
{
    char FilePath[1024] = {};
    StringCchCatA(FilePath, ArrayCount(FilePath), BuildContext->EnvironmentInfo.RootDirectoryPath);
    StringCchCatA(FilePath, ArrayCount(FilePath), "\\configuration\\win32\\tools\\build\\targets\\lint\\headers.json");

    read_file_result File = ReadFileIntoMemory(FilePath);
    cJSON *Json = cJSON_ParseWithLength((char *)File.FileMemory, File.Size);
    FreeFileMemory(File);
}

b32 BuildLintOptimized(build_context *BuildContext)
{
    MakeBuildGraph(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tools\\lint\\lint.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\libraries\\file_system\\files.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\libraries\\strings\\path_handling.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /GR- /EHa- /fp:fast /fp:except-");
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

    AddLinkerInputFile(BuildContext, "\\lint.obj");
    AddLinkerInputFile(BuildContext, "\\files.obj");
    AddLinkerInputFile(BuildContext, "\\path_handling.obj");
    AddLinkerFlags(BuildContext, "/NOLOGO /DEBUG /subsystem:console /incremental:no /opt:ref /NODEFAULTLIB:LIBCMT");
    AddLinkerFlags(BuildContext, "Shlwapi.lib libcmtd.lib");
    SetLinkerOutputBinary(BuildContext, "\\lint.exe");
    BuildSuccess = LinkWithMSVC(BuildContext);

    return BuildSuccess;
}

b32 BuildLint(build_context *BuildContext)
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