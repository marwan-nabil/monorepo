static void AddCompilerFlags(build_context *BuildContext, const char *Flags)
{
    StringCchCatA(BuildContext->CompilerFlags, ArrayCount(BuildContext->CompilerFlags), Flags);
    StringCchCatA(BuildContext->CompilerFlags, ArrayCount(BuildContext->CompilerFlags), " ");
}

static void AddLinkerFlags(build_context *BuildContext, const char *Flags)
{
    StringCchCatA(BuildContext->LinkerFlags, ArrayCount(BuildContext->LinkerFlags), Flags);
    StringCchCatA(BuildContext->LinkerFlags, ArrayCount(BuildContext->LinkerFlags), " ");
}

static void AddSourceFile(build_context *BuildContext, const char *SourceFile)
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

static void AddLinkerInputFile(build_context *BuildContext, const char *LinkerInputFile)
{
    StringCchCatA
    (
        BuildContext->LinkerInputsString,
        ArrayCount(BuildContext->LinkerInputsString),
        BuildContext->TargetOutputDirectoryPath
    );
    StringCchCatA(BuildContext->LinkerInputsString, ArrayCount(BuildContext->LinkerInputsString), LinkerInputFile);
    StringCchCatA(BuildContext->LinkerInputsString, ArrayCount(BuildContext->LinkerInputsString), " ");
}

static void SetOuputBinaryPath(build_context *BuildContext, const char *OutputBinaryName)
{
    StringCchCatA
    (
        BuildContext->OutputBinaryPath,
        ArrayCount(BuildContext->OutputBinaryPath),
        BuildContext->TargetOutputDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->OutputBinaryPath,
        ArrayCount(BuildContext->OutputBinaryPath),
        OutputBinaryName
    );
}

static void ClearBuildContext(build_context *BuildContext)
{
    *BuildContext->CompilerFlags = {};
    *BuildContext->LinkerFlags = {};
    *BuildContext->OutputBinaryPath = {};
    *BuildContext->SourcesString = {};
    *BuildContext->LinkerInputsString = {};
}

static void DisplayHelp()
{
    printf("INFO: Available build targets:\n");
    printf("          build help\n");
    printf("          build clean\n");
    printf("          build clean_all\n");

    printf("          build lint [job_per_directory]\n");
    printf("          build fetch_data\n");
    printf("          build compilation_tests\n");
    printf("          build fat12_tests\n");
    printf("          build simulator\n");
    printf("          build directx_demo [debug, release]\n");
    printf("          build handmade_hero\n");
    printf("          build imgui_demo [opengl2, dx11]\n");
    printf("          build ray_tracer [1_lane, 4_lanes, 8_lanes]\n");
    printf("          build x86_kernel\n");
    printf("          build x86_kernel_tests\n");
}