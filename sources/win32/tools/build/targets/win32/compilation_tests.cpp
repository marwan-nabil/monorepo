#include <Windows.h>
#include <stdint.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\console\console.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

b32 BuildCompilationTests(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\tests\\compilation_tests\\compilation_tests.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\system\\processes.cpp");
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\console\\console.cpp");

    AddCompilerFlags(BuildContext, "/nologo /FC /O2 /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");
    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");
    SetLinkerOutputBinary(BuildContext, "\\compilation_tests.exe");
    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}