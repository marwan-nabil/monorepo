static b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\assembly.s");
    AddCompilerFlags(BuildContext, "-f elf");
    SetCompilerOutputObject(BuildContext, "\\assembly.elf");
    SetCompilerIncludePath(BuildContext, "\\");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=kernel.map");
    SetLinkerScriptPath(BuildContext, "\\projects\\os\\kernel\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\assembly.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetLinkerOutputBinary(BuildContext, "\\kernel.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}