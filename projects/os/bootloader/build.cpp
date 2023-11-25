static b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");

    AddSourceFile(BuildContext, "\\projects\\os\\bootloader\\entry.s");
    SetOuputBinaryPath(BuildContext, "\\entry.elf");
    AddCompilerFlags(BuildContext, "-f elf");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\os\\bootloader\\main.c");
    SetOuputBinaryPath(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=bootloader.map");
    AddSourceFile(BuildContext, "\\projects\\os\\bootloader\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\entry.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetOuputBinaryPath(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}