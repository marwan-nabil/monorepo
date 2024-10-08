#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "win32\libraries\base_types.h"
#include "win32\libraries\basic_defines.h"
#include "win32\libraries\shell\console.h"
#include "win32\libraries\system\processes.h"
#include "win32\libraries\strings\strings.h"
#include "win32\libraries\strings\string_list.h"

#include "win32\tools\build\actions\build_context.h"
#include "win32\tools\build\actions\nasm.h"

b32 AssembleWithNasm(build_context *BuildContext)
{
    char AssemblerCommand[1024];
    *AssemblerCommand = {};
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "nasm.exe ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.CompilerFlags);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "-i ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.CompilerIncludePath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " ");
    FlattenStringList(BuildContext->CompilationInfo.Sources, AssemblerCommand, ArrayCount(AssemblerCommand));
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " -o \"");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.OutputObjectPath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "\" ");

    b32 Result = CreateProcessAndWait(AssemblerCommand);
    if (!Result)
    {
        ConsolePrintColored
        (
            "ERROR: Assembly failed.\n",
            FOREGROUND_RED
        );
    }

    ClearBuildContext(BuildContext); // TODO: remove from here
    return Result;
}