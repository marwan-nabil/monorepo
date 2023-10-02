inline void AddCompilerFlags(build_context *BuildContext, const char *Flags)
{
    StringCchCatA(BuildContext->CompilerFlags, ArrayCount(BuildContext->CompilerFlags), Flags);
    StringCchCatA(BuildContext->CompilerFlags, ArrayCount(BuildContext->CompilerFlags), " ");
}

inline void AddLinkerFlags(build_context *BuildContext, const char *Flags)
{
    StringCchCatA(BuildContext->LinkerFlags, ArrayCount(BuildContext->LinkerFlags), Flags);
    StringCchCatA(BuildContext->LinkerFlags, ArrayCount(BuildContext->LinkerFlags), " ");
}

inline void AddSourceFile(build_context *BuildContext, const char *SourceFile)
{
    StringCchCatA
    (
        BuildContext->SourcesString,
        ArrayCount(BuildContext->SourcesString),
        BuildContext->RootDirectoryPath
    );
    StringCchCatA(BuildContext->SourcesString, ArrayCount(BuildContext->SourcesString), SourceFile);
    StringCchCatA(BuildContext->SourcesString, ArrayCount(BuildContext->SourcesString), " ");
}

inline void SetOuputBinaryPath(build_context *BuildContext, const char *OutputBinaryName)
{
    StringCchCatA
    (
        BuildContext->OutputBinaryPath,
        ArrayCount(BuildContext->OutputBinaryPath),
        BuildContext->OutputDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->OutputBinaryPath,
        ArrayCount(BuildContext->OutputBinaryPath),
        OutputBinaryName
    );
}

inline void ClearBuildContext(build_context *BuildContext)
{
    ZeroMemory(BuildContext->CompilerFlags, ArrayCount(BuildContext->CompilerFlags));
    ZeroMemory(BuildContext->LinkerFlags, ArrayCount(BuildContext->LinkerFlags));
    ZeroMemory(BuildContext->OutputBinaryPath, ArrayCount(BuildContext->OutputBinaryPath));
    ZeroMemory(BuildContext->SourcesString, ArrayCount(BuildContext->SourcesString));
}

void DisplayHelp()
{
    printf("INFO: Available build targets:\n");
    printf("          build help\n");
    printf("          build clean\n");
    printf("          build build\n");
    printf("          build basic_app\n");
    printf("          build handmade_hero\n");
    printf("          build directx_demo [debug, release]\n");
    printf("          build imgui_demo [opengl2, dx11]\n");
    printf("          build ray_tracer [optimized, non_optimized] [1_lane, 4_lanes, 8_lanes]\n");
    printf("          build lint [job_per_directory]\n");
    printf("          build x86_kernel\n");
    printf("          build compilation_tests\n");
    printf("          build fat12_tests\n");
    printf("          build x86_kernel_tests\n");
}

b32 CompileShader(build_context *BuildContext)
{
    char CompilerCommand[1024];
    ZeroMemory(CompilerCommand, ArrayCount(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "fxc.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fo \"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->SourcesString);

    b32 Result = CreateProcessAndWait(CompilerCommand);
    if (!Result)
    {
        ConsolePrintColored("ERROR: shader compilation failed.\n", FOREGROUND_RED);
    }

    return Result;
}

b32 CompileCpp(build_context *BuildContext)
{
    char CompilerCommand[1024];
    ZeroMemory(CompilerCommand, ArrayCount(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "cl.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->SourcesString);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fe:\"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "/link ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), BuildContext->LinkerFlags);

    b32 Result = CreateProcessAndWait(CompilerCommand);
    if (!Result)
    {
        ConsolePrintColored("ERROR: compilation failed.\n", FOREGROUND_RED);
    }

    return Result;
}

b32 CompileAssembly(build_context *BuildContext)
{
    char AssemblerCommand[1024];
    ZeroMemory(AssemblerCommand, ArrayCount(AssemblerCommand));
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "nasm.exe ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->CompilerFlags);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->SourcesString);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " -o \"");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), BuildContext->OutputBinaryPath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "\" ");

    b32 Result = CreateProcessAndWait(AssemblerCommand);
    if (!Result)
    {
        ConsolePrintColored("ERROR: Assembly failed.\n", FOREGROUND_RED);
    }

    return Result;
}