#include <Windows.h>
#include <stdint.h>
#include "platforms\win32\libraries\base_types.h"
#include "platforms\win32\libraries\basic_defines.h"
#include "platforms\win32\libraries\strings\string_list.h"

#include "platforms\win32\tools\build\actions\build_context.h"
#include "platforms\win32\tools\build\actions\nasm.h"

b32 BuildBootSectorImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootsector");
    AddCompilerSourceFile(BuildContext, "\\platforms\\i686-elf\\bootsector\\entry.s");
    AddCompilerFlags(BuildContext, "-f bin -lboot_sector.lst");
    SetCompilerIncludePath(BuildContext, "\\");
    SetCompilerOutputObject(BuildContext, "\\bootsector.img");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    PopSubTarget(BuildContext);
    return BuildSuccess;
}