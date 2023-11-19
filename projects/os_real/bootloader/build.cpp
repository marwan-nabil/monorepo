static b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");

#if 0
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
#endif

    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\main.c");
    SetOuputBinaryPath(BuildContext, "\\main.obj");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    b32 BuildSuccess = CompilerWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

#if 0
    AddLinkerFlags(BuildContext, "-nostdlib -lgcc -Wl,-Map=bootloader.map");
    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\linker.ls");
    AddLinkerInputFile(BuildContext, "\\*.obj");
    SetOuputBinaryPath(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithGCC(BuildContext);
#endif

    PopSubTarget(BuildContext);
    return BuildSuccess;
}