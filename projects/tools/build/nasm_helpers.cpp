static b32 AssembleWithNasm(build_context *BuildContext)
{
    char AssemblerCommand[1024];
    *AssemblerCommand = {};
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "nasm.exe ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.CompilerFlags);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "-i \"");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.CompilerIncludePath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "\" ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilationInfo.SourcesString);
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

    return Result;
}