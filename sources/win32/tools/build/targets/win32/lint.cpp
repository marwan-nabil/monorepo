#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\string_list.h"
#include "sources\win32\libraries\file_system\files.h"
#include "sources\win32\libraries\file_system\folders.h"

#include "..\..\graph\loading.h"
#include "..\..\actions\build_context.h"
#include "..\..\actions\msvc.h"

b32 BuildLintOptimized(build_context *BuildContext)
{
    char FolderPath[1024] = {};
    StringCchCatA(FolderPath, ArrayCount(FolderPath), BuildContext->EnvironmentInfo.BuildConfigurationDirectoryPath);
    StringCchCatA(FolderPath, ArrayCount(FolderPath), "\\win32\\lint");
    string_node *FilePathsList = GetListOfFilesInFolder(FolderPath);
    string_node *CurrentNode = FilePathsList;
    while (CurrentNode)
    {
        LoadFileIntoTargetGraph(CurrentNode->String, BuildContext->EnvironmentInfo.BuildConfigurationDirectoryPath);
        CurrentNode = CurrentNode->NextString;
    }
    FreeStringList(FilePathsList);
    return TRUE;
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