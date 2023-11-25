static void AddCompilerFlags(build_context *BuildContext, const char *Flags)
{
    StringCchCatA
    (
        BuildContext->CompilationInfo.CompilerFlags,
        ArrayCount(BuildContext->CompilationInfo.CompilerFlags),
        Flags
    );
    StringCchCatA
    (
        BuildContext->CompilationInfo.CompilerFlags,
        ArrayCount(BuildContext->CompilationInfo.CompilerFlags),
        " "
    );
}

static void SetCompilerIncludePath(build_context *BuildContext, const char *IncludePath)
{
    StringCchCatA
    (
        BuildContext->CompilationInfo.CompilerIncludePath,
        ArrayCount(BuildContext->CompilationInfo.CompilerIncludePath),
        BuildContext->EnvironmentInfo.RootDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->CompilationInfo.CompilerIncludePath,
        ArrayCount(BuildContext->CompilationInfo.CompilerIncludePath),
        IncludePath
    );
}

static void AddCompilerSourceFile(build_context *BuildContext, const char *SourceFile)
{
    StringCchCatA
    (
        BuildContext->CompilationInfo.SourcesString,
        ArrayCount(BuildContext->CompilationInfo.SourcesString),
        BuildContext->EnvironmentInfo.RootDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->CompilationInfo.SourcesString,
        ArrayCount(BuildContext->CompilationInfo.SourcesString),
        SourceFile
    );
    StringCchCatA
    (
        BuildContext->CompilationInfo.SourcesString,
        ArrayCount(BuildContext->CompilationInfo.SourcesString),
        " "
    );
}

static void SetCompilerOutputObject(build_context *BuildContext, const char *ObjectFile)
{
    StringCchCatA
    (
        BuildContext->CompilationInfo.OutputObjectPath,
        ArrayCount(BuildContext->CompilationInfo.OutputObjectPath),
        BuildContext->EnvironmentInfo.TargetOutputDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->CompilationInfo.OutputObjectPath,
        ArrayCount(BuildContext->CompilationInfo.OutputObjectPath),
        ObjectFile
    );
}

static void AddLinkerFlags(build_context *BuildContext, const char *Flags)
{
    StringCchCatA
    (
        BuildContext->LinkingInfo.LinkerFlags,
        ArrayCount(BuildContext->LinkingInfo.LinkerFlags),
        Flags
    );
    StringCchCatA
    (
        BuildContext->LinkingInfo.LinkerFlags,
        ArrayCount(BuildContext->LinkingInfo.LinkerFlags),
        " "
    );
}

static void SetLinkerScriptPath(build_context *BuildContext, const char *LinkerScriptFile)
{
    StringCchCatA
    (
        BuildContext->LinkingInfo.LinkerScriptPath,
        ArrayCount(BuildContext->LinkingInfo.LinkerScriptPath),
        BuildContext->EnvironmentInfo.RootDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->LinkingInfo.LinkerScriptPath,
        ArrayCount(BuildContext->LinkingInfo.LinkerScriptPath),
        LinkerScriptFile
    );
}

static void AddLinkerInputFile(build_context *BuildContext, const char *LinkerInputFile)
{
    StringCchCatA
    (
        BuildContext->LinkingInfo.LinkerInputsString,
        ArrayCount(BuildContext->LinkingInfo.LinkerInputsString),
        BuildContext->EnvironmentInfo.TargetOutputDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->LinkingInfo.LinkerInputsString,
        ArrayCount(BuildContext->LinkingInfo.LinkerInputsString),
        LinkerInputFile
    );
    StringCchCatA
    (
        BuildContext->LinkingInfo.LinkerInputsString,
        ArrayCount(BuildContext->LinkingInfo.LinkerInputsString),
        " "
    );
}

static void SetLinkerOutputBinary(build_context *BuildContext, const char *OutputBinaryPath)
{
    StringCchCatA
    (
        BuildContext->LinkingInfo.OutputBinaryPath,
        ArrayCount(BuildContext->LinkingInfo.OutputBinaryPath),
        BuildContext->EnvironmentInfo.TargetOutputDirectoryPath
    );
    StringCchCatA
    (
        BuildContext->LinkingInfo.OutputBinaryPath,
        ArrayCount(BuildContext->LinkingInfo.OutputBinaryPath),
        OutputBinaryPath
    );
}

static void PushSubTarget(build_context *BuildContext, const char *SubTargetRelativePath)
{
    StringCchCatA
    (
        BuildContext->EnvironmentInfo.TargetOutputDirectoryPath,
        ArrayCount(BuildContext->EnvironmentInfo.TargetOutputDirectoryPath),
        "\\"
    );
    StringCchCatA
    (
        BuildContext->EnvironmentInfo.TargetOutputDirectoryPath,
        ArrayCount(BuildContext->EnvironmentInfo.TargetOutputDirectoryPath),
        SubTargetRelativePath
    );

    CreateDirectoryA(BuildContext->EnvironmentInfo.TargetOutputDirectoryPath, NULL);
    SetCurrentDirectory(BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
}

static void PopSubTarget(build_context *BuildContext)
{
    RemoveLastSegmentFromPath(BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    SetCurrentDirectory(BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
}

static void ClearBuildContext(build_context *BuildContext)
{
    BuildContext->CompilationInfo = {};
    BuildContext->LinkingInfo = {};
}