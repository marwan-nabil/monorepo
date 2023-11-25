static b32 CompileWithGCC(build_context *BuildContext)
{
    char CompilerCommand[KiloBytes(2)];
    *CompilerCommand = {};
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "i686-elf-gcc.exe -c ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " -I\"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.CompilerIncludePath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "-o ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.OutputObjectPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilationInfo.SourcesString);

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

    return Result;
}

static b32 LinkWithGCC(build_context *BuildContext)
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
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->LinkingInfo.LinkerInputsString);
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

    return Result;
}