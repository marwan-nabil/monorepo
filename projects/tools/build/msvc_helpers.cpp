static b32 CompileShader(build_context *BuildContext)
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

    return Result;
}

static b32 CompileWithMSVC(build_context *BuildContext)
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

    return Result;
}