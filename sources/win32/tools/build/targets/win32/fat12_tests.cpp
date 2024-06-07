#include <Windows.h>
#include <stdint.h>
#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\string_list.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\win32_compiler_helpers.h"

b32 BuildFat12Tests(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tests\\win32\\fat12\\test.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\libraries\\shell\\console.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\libraries\\strings\\path_handling.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\libraries\\file_system\\fat12\\*.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");
    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");
    SetLinkerOutputBinary(BuildContext, "\\fat12_tests.exe");

    b32 BuildSuccess = CompileAndLinkWithMSVC(BuildContext);
    return BuildSuccess;
}