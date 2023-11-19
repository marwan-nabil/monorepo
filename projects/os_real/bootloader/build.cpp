static b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");

    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\entry.s");
    SetOuputBinaryPath(BuildContext, "\\entry.elf");
    AddCompilerFlags(BuildContext, "-f elf");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\main.c");
    SetOuputBinaryPath(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    BuildSuccess = CompilerWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerFlags(BuildContext, "-nostdlib -lgcc -Wl,-Map=bootloader.map");
    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\entry.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetOuputBinaryPath(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}