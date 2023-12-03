static b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");

    char *SharedCCFlags = "-std=c99 -g -ffreestanding -nostdlib";
    char *SharedAsmFlags = "-f elf";

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\platform.s");
    AddCompilerFlags(BuildContext, SharedAsmFlags);
    SetCompilerOutputObject(BuildContext, "\\platform_s.elf");
    SetCompilerIncludePath(BuildContext, "\\");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\isr.s");
    AddCompilerFlags(BuildContext, SharedAsmFlags);
    SetCompilerOutputObject(BuildContext, "\\isr_s.elf");
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\platform.c");
    SetCompilerOutputObject(BuildContext, "\\platform_c.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\isr.c");
    SetCompilerOutputObject(BuildContext, "\\isr_c.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\descriptor_tables.c");
    SetCompilerOutputObject(BuildContext, "\\descriptor_tables.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\tests.c");
    SetCompilerOutputObject(BuildContext, "\\tests.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\kernel\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=kernel.map");
    SetLinkerScriptPath(BuildContext, "\\projects\\os\\kernel\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\platform_s.elf");
    AddLinkerInputFile(BuildContext, "\\platform_c.elf");
    AddLinkerInputFile(BuildContext, "\\isr_s.elf");
    AddLinkerInputFile(BuildContext, "\\isr_c.elf");
    AddLinkerInputFile(BuildContext, "\\descriptor_tables.elf");
    AddLinkerInputFile(BuildContext, "\\tests.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetLinkerOutputBinary(BuildContext, "\\kernel.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}