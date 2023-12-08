static b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\bootloader\\entry.s");
    SetCompilerOutputObject(BuildContext, "\\entry.elf");
    AddCompilerFlags(BuildContext, "-f elf");
    SetCompilerIncludePath(BuildContext, "\\");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\i686-elf\\bootloader\\main.c");
    SetCompilerOutputObject(BuildContext, "\\main.elf");
    AddCompilerFlags(BuildContext, "-std=c99 -g -ffreestanding -nostdlib");
    SetCompilerIncludePath(BuildContext, "\\");
    BuildSuccess = CompileWithGCC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddLinkerFlags(BuildContext, "-nostdlib -Wl,-Map=bootloader.map");
    SetLinkerScriptPath(BuildContext, "\\sources\\i686-elf\\bootloader\\linker.lds");
    AddLinkerInputFile(BuildContext, "\\entry.elf");
    AddLinkerInputFile(BuildContext, "\\main.elf");
    SetLinkerOutputBinary(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithGCC(BuildContext);

    PopSubTarget(BuildContext);
    return BuildSuccess;
}