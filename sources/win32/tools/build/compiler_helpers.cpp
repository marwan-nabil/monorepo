#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\shared\base_types.h"
#include "sources\win32\shared\basic_defines.h"
#include "sources\win32\shared\console\console.h"
#include "sources\win32\shared\system\processes.h"
#include "build.h"
#include "build_helpers.h"


b32 CompileWithGCC(build_context *BuildContext)
{
    char CompilerCommand[KiloBytes(2)];
    *CompilerCommand = {};
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "i686-elf-gcc.exe -c ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " -I ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.CompilerIncludePath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "-o ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.OutputObjectPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    FlattenFileNameList(BuildContext->CompilationInfo.Sources, CompilerCommand, ArrayCount(CompilerCommand));

    b32 Result = CreateProcessAndWait
    (
        CompilerCommand,
        BuildContext->EnvironmentInfo.ConsoleContext
    );
    if (!Result)
    {
        ConsolePrintColored
        (
            "ERROR: compilation failed.\n",
            BuildContext->EnvironmentInfo.ConsoleContext,
            FOREGROUND_RED
        );
    }

    ClearBuildContext(BuildContext);
    return Result;
}

b32 LinkWithGCC(build_context *BuildContext)
{
    char LinkerCommand[1024];
    *LinkerCommand = {};

    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), "i686-elf-gcc.exe ");
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->LinkingInfo.LinkerFlags);
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " -T ");
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->LinkingInfo.LinkerScriptPath);
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " -o ");
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->LinkingInfo.OutputBinaryPath);
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " ");
    FlattenFileNameList(BuildContext->LinkingInfo.LinkerInputs, LinkerCommand, ArrayCount(LinkerCommand));
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " -lgcc");

    b32 Result = CreateProcessAndWait
    (
        LinkerCommand,
        BuildContext->EnvironmentInfo.ConsoleContext
    );
    if (!Result)
    {
        ConsolePrintColored
        (
            "ERROR: linking failed.\n",
            BuildContext->EnvironmentInfo.ConsoleContext,
            FOREGROUND_RED
        );
    }

    ClearBuildContext(BuildContext);
    return Result;
}


b32 CompileShader(build_context *BuildContext)
{
    char CompilerCommand[1024];
    *CompilerCommand = {};
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "fxc.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fo \"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.OutputObjectPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    FlattenFileNameList(BuildContext->CompilationInfo.Sources, CompilerCommand, ArrayCount(CompilerCommand));

    b32 Result = CreateProcessAndWait(CompilerCommand, BuildContext->EnvironmentInfo.ConsoleContext);
    if (!Result)
    {
        ConsolePrintColored
        (
            "ERROR: shader compilation failed.\n",
            BuildContext->EnvironmentInfo.ConsoleContext,
            FOREGROUND_RED
        );
    }

    ClearBuildContext(BuildContext);
    return Result;
}

b32 CompileWithMSVC(build_context *BuildContext)
{
    char CompilerCommand[KiloBytes(2)];
    *CompilerCommand = {};
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "cl.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /I");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.CompilerIncludePath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    FlattenFileNameList(BuildContext->CompilationInfo.Sources, CompilerCommand, ArrayCount(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fe:\"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->LinkingInfo.OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "/link ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->LinkingInfo.LinkerFlags);

    b32 Result = CreateProcessAndWait
    (
        CompilerCommand,
        BuildContext->EnvironmentInfo.ConsoleContext
    );
    if (!Result)
    {
        ConsolePrintColored
        (
            "ERROR: compilation failed.\n",
            BuildContext->EnvironmentInfo.ConsoleContext,
            FOREGROUND_RED
        );
    }

    ClearBuildContext(BuildContext);
    return Result;
}

b32 AssembleWithNasm(build_context *BuildContext)
{
    char AssemblerCommand[1024];
    *AssemblerCommand = {};
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "nasm.exe ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.CompilerFlags);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "-i ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.CompilerIncludePath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " ");
    FlattenFileNameList(BuildContext->CompilationInfo.Sources, AssemblerCommand, ArrayCount(AssemblerCommand));
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " -o \"");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.OutputObjectPath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "\" ");

    b32 Result = CreateProcessAndWait
    (
        AssemblerCommand,
        BuildContext->EnvironmentInfo.ConsoleContext
    );
    if (!Result)
    {
        ConsolePrintColored
        (
            "ERROR: Assembly failed.\n",
            BuildContext->EnvironmentInfo.ConsoleContext,
            FOREGROUND_RED
        );
    }

    ClearBuildContext(BuildContext);
    return Result;
}