static b32 BuildKernelImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "kernel");

    char *SharedCCFlags = "-std=c99 -g -ffreestanding -nostdlib";
    char *SharedAsmFlags = "-f elf";

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\platform.s");
    AddCompilerFlags(BuildContext, SharedAsmFlags);
    SetCompilerOutputObject(BuildContext, "\\platform_s.elf");
    SetCompilerIncludePath(BuildContext, "\\sources\\i686-elf");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\isr.s");
    AddCompilerFlags(BuildContext, SharedAsmFlags);
    SetCompilerOutputObject(BuildContext, "\\isr_s.elf");
    SetCompilerIncludePath(BuildContext, "\\sources\\i686-elf");
    BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\platform.c");
    SetCompilerOutputObject(BuildContext, "\\platform_c.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\sources\\i686-elf");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\isr.c");
    SetCompilerOutputObject(BuildContext, "\\isr_c.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\sources\\i686-elf");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\descriptor_tables.c");
    SetCompilerOutputObject(BuildContext, "\\descriptor_tables.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\sources\\i686-elf");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\tests.c");
    SetCompilerOutputObject(BuildContext, "\\tests.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\sources\\i686-elf");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\kernel\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, SharedCCFlags);
    SetCompilerIncludePath(BuildContext, "\\sources\\i686-elf");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=kernel.map");
    SetLinkerScriptPath(BuildContext, "\\sources\\i686-elf\\kernel\\linker.lds");
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