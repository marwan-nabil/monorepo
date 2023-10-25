static b32 CompileWithWatcom(build_context *BuildContext)
{
    char CompilerCommand[1024];
    *CompilerCommand = {};
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "wcc.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->SourcesString);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " -fo=\"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");

    b32 Result = CreateProcessAndWait(CompilerCommand, BuildContext->ConsoleContext);
    if (!Result)
    {
        ConsolePrintColored("ERROR: compilation failed.\n", BuildContext->ConsoleContext, FOREGROUND_RED);
    }

    return Result;
}

static b32 LinkWithWatcom(build_context *BuildContext)
{
    char CompilerCommand[1024];
    *CompilerCommand = {};

    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "wlink.exe NAME ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " FILE {");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->LinkerInputsString);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "} ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->LinkerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " @");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->SourcesString);

    b32 Result = CreateProcessAndWait(CompilerCommand, BuildContext->ConsoleContext);
    if (!Result)
    {
        ConsolePrintColored("ERROR: linking failed.\n", BuildContext->ConsoleContext, FOREGROUND_RED);
    }

    return Result;
}