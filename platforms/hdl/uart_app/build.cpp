#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "platforms\win32\libraries\base_types.h"
#include "platforms\win32\libraries\basic_defines.h"
#include "platforms\win32\libraries\strings\string_list.h"

#include "platforms\win32\tools\build\actions\build_context.h"
#include "platforms\win32\tools\build\actions\iverilog.h"

b32 BuildUARTApp(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\platforms\\hdl\\uart_app\\uart\\uart_test.v");
    SetCompilerIncludePath(BuildContext, "\\platforms\\hdl\\uart_app\\uart");
    SetCompilerOutputObject(BuildContext, "\\uart_app.vvp");
    b32 BuildSuccess = CompileWithIVerilog(BuildContext);
    ClearBuildContext(BuildContext);
    return BuildSuccess;
}