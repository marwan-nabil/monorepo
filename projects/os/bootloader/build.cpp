static b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\bootloader\\entry.s");
    SetCompilerOutputObject(BuildContext, "\\entry.elf");
    AddCompilerFlags(BuildContext, "-f elf");
    SetCompilerIncludePath(BuildContext, BuildContext->EnvironmentInfo.RootDirectoryPath);
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddCompilerSourceFile(BuildContext, "\\projects\\os\\bootloader\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, BuildContext->EnvironmentInfo.RootDirectoryPath);
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=bootloader.map");
    SetLinkerScriptPath(BuildContext, "\\projects\\os\\bootloader\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\entry.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetLinkerOutputBinary(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}