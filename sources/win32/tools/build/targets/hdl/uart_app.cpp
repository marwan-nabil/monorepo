#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\string_list.h"

#include "..\..\actions\build_context.h"
#include "..\..\actions\iverilog.h"

b32 BuildUARTApp(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\hdl\\uart_app\\uart\\uart_test.v");
    SetCompilerIncludePath(BuildContext, "\\sources\\hdl\\uart_app\\uart");
    SetCompilerOutputObject(BuildContext, "\\uart_app.vvp");
    b32 BuildSuccess = CompileWithIVerilog(BuildContext);
    return BuildSuccess;
}