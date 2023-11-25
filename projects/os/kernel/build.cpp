static b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\entry.s");
    AddCompilerFlags(BuildContext, "-f elf");
    SetCompilerOutputObject(BuildContext, "\\entry.elf");
    SetCompilerIncludePath(BuildContext, BuildContext->EnvironmentInfo.RootDirectoryPath);
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, BuildContext->EnvironmentInfo.RootDirectoryPath);
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=kernel.map");
    SetLinkerScriptPath(BuildContext, "\\projects\\os\\kernel\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\entry.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetLinkerOutputBinary(BuildContext, "\\kernel.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}