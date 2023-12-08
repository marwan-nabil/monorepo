#include <Windows.h>
#include <stdint.h>

#include "sources\win32\shared\base_types.h"
#include "sources\win32\shared\basic_defines.h"
#include "sources\win32\shared\console\console.h"
#include "sources\win32\tools\build_dynamic\build.h"
#include "sources\win32\tools\build_dynamic\build_helpers.h"
#include "sources\win32\tools\build_dynamic\msvc_helpers.h"

b32 BuildSimulator(build_context *BuildContext);

build_target_config BuildConfiguration = {"simulator", &BuildSimulator, NULL, NULL, NULL};

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