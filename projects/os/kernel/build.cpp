static b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");

    AddSourceFile(BuildContext, "\\projects\\os\\kernel\\entry.s");
    AddCompilerFlags(BuildContext, "-f elf");
    SetOuputBinaryPath(BuildContext, "\\entry.elf");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\os\\kernel\\main.c");
    SetOuputBinaryPath(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=kernel.map");
    AddSourceFile(BuildContext, "\\projects\\os\\kernel\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\entry.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetOuputBinaryPath(BuildContext, "\\kernel.img");
    BuildSuccess = LinkWithGCC(BuildContext);
    
    PopSubTarget(BuildContext);
    return BuildSuccess;
}