static b32 CompilerWithGCC(build_context *BuildContext)
{
    char CompilerCommand[1024];
    *CompilerCommand = {};
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "i686-elf-gcc.exe -c ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " -I\"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilerIncludePath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "-o ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->SourcesString);

    b32 Result = CreateProcessAndWait(CompilerCommand, BuildContext->ConsoleContext);
    if (!Result)
    {
        ConsolePrintColored("ERROR: compilation failed.\n", BuildContext->ConsoleContext, FOREGROUND_RED);
    }

    return Result;
}

static b32 LinkWithGCC(build_context *BuildContext)
{
    char LinkerCommand[1024];
    *LinkerCommand = {};

    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), "i686-elf-gcc.exe ");
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->LinkerFlags);
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " -T ");
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->SourcesString);
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " -o ");
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " ");
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), BuildContext->LinkerInputsString);
    StringCchCatA(LinkerCommand, ArrayCount(LinkerCommand), " -lgcc");

    b32 Result = CreateProcessAndWait(LinkerCommand, BuildContext->ConsoleContext);
    if (!Result)
    {
        ConsolePrintColored("ERROR: linking failed.\n", BuildContext->ConsoleContext, FOREGROUND_RED);
    }

    return Result;
}