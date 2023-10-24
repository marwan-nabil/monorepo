static b32 AssembleWithNasm(build_context *BuildContext)
{
    char AssemblerCommand[1024];
    *AssemblerCommand = {};
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "nasm.exe ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilerFlags);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->SourcesString);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " -o \"");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "\" ");

    b32 Result = CreateProcessAndWait(AssemblerCommand, BuildContext->ConsoleContext);
    if (!Result)
    {
        ConsolePrintColored("ERROR: Assembly failed.\n", BuildContext->ConsoleContext, FOREGROUND_RED);
    }

    return Result;
}