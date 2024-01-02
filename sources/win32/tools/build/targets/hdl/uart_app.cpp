#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\strings\string_list.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

b32 BuildUARTApp(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\hdl\\uart_app\\uart\\baud_generator_test.v");
    SetCompilerIncludePath(BuildContext, "\\sources\\hdl\\uart_app\\uart");
    SetCompilerOutputObject(BuildContext, "\\uart_app.vvp");
    b32 BuildSuccess = CompileWithIVerilog(BuildContext);
    return BuildSuccess;
}