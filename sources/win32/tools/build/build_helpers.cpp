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
    // TODO: make this a list instead of a single string
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
    file_name_node *NewNode = (file_name_node *)malloc(sizeof(file_name_node));
    ZeroMemory(NewNode->FileName, MAX_FILENAME);
    NewNode->NextFileName = BuildContext->CompilationInfo.Sources;
    BuildContext->CompilationInfo.Sources = NewNode;

    StringCchCatA
    (
        NewNode->FileName,
        ArrayCount(NewNode->FileName),
        BuildContext->EnvironmentInfo.RootDirectoryPath
    );
    StringCchCatA
    (
        NewNode->FileName,
        ArrayCount(NewNode->FileName),
        SourceFile
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
    file_name_node *NewNode = (file_name_node *)malloc(sizeof(file_name_node));
    ZeroMemory(NewNode->FileName, MAX_FILENAME);
    NewNode->NextFileName = BuildContext->LinkingInfo.LinkerInputs;
    BuildContext->LinkingInfo.LinkerInputs = NewNode;

    StringCchCatA
    (
        NewNode->FileName,
        ArrayCount(NewNode->FileName),
        BuildContext->EnvironmentInfo.TargetOutputDirectoryPath
    );
    StringCchCatA
    (
        NewNode->FileName,
        ArrayCount(NewNode->FileName),
        LinkerInputFile
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

static void
FreeFileNameList(file_name_node *RootNode)
{
    file_name_node *CurrentNode = RootNode;
    file_name_node *ChildNode;

    while (CurrentNode)
    {
        ChildNode = CurrentNode->NextFileName;
        free(CurrentNode);
        CurrentNode = ChildNode;
    }
}

static void
FlattenFileNameList(file_name_node *FileNameList, char *Output, u32 OutputSize)
{
    while (FileNameList)
    {
        StringCchCatA(Output, OutputSize, FileNameList->FileName);
        StringCchCatA(Output, OutputSize, " ");
        FileNameList = FileNameList->NextFileName;
    }
}

static void ClearBuildContext(build_context *BuildContext)
{
    FreeFileNameList(BuildContext->CompilationInfo.Sources);
    FreeFileNameList(BuildContext->LinkingInfo.LinkerInputs);
    BuildContext->CompilationInfo = {};
    BuildContext->LinkingInfo = {};
}