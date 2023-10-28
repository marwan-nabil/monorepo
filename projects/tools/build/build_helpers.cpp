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

static void SetOuputBinaryPath(build_context *BuildContext, const char *OutputBinaryPath)
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
        OutputBinaryPath
    );
}

static void SetCompilerIncludePath(build_context *BuildContext, const char *IncludePath)
{
    StringCchCatA
    (
        BuildContext->CompilerIncludePath,
        ArrayCount(BuildContext->CompilerIncludePath),
        IncludePath
    );
}

static void PushSubTarget(build_context *BuildContext, const char *SubTargetRelativePath)
{
    StringCchCatA
    (
        BuildContext->TargetOutputDirectoryPath,
        ArrayCount(BuildContext->TargetOutputDirectoryPath),
        "\\"
    );
    StringCchCatA
    (
        BuildContext->TargetOutputDirectoryPath,
        ArrayCount(BuildContext->TargetOutputDirectoryPath),
        SubTargetRelativePath
    );

    CreateDirectoryA(BuildContext->TargetOutputDirectoryPath, NULL);
    SetCurrentDirectory(BuildContext->TargetOutputDirectoryPath);
}

static void PopSubTarget(build_context *BuildContext)
{
    RemoveLastSegmentFromPath(BuildContext->TargetOutputDirectoryPath);
    SetCurrentDirectory(BuildContext->TargetOutputDirectoryPath);
}

static void ClearBuildContext(build_context *BuildContext)
{
    *BuildContext->CompilerFlags = {};
    *BuildContext->LinkerFlags = {};
    *BuildContext->OutputBinaryPath = {};
    *BuildContext->CompilerIncludePath = {};
    *BuildContext->SourcesString = {};
    *BuildContext->LinkerInputsString = {};
}