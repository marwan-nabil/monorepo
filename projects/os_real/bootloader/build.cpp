static b32 BuildBootloaderImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "bootloader");

    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\entry.s");
    SetOuputBinaryPath(BuildContext, "\\entry.obj");
    AddCompilerFlags(BuildContext, "-f obj");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\main.c");
    SetOuputBinaryPath(BuildContext, "\\main.obj");
    AddCompilerFlags(BuildContext, "-4 -d3 -s -ms -zl -zq -za99");
    AddCompilerFlags(BuildContext, "-wx -wcd=138 -wcd=202");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    BuildSuccess = CompileWithWatcom(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    AddLinkerInputFile(BuildContext, "\\*.obj");
    AddSourceFile(BuildContext, "\\projects\\os_real\\bootloader\\linker.ls");
    SetOuputBinaryPath(BuildContext, "\\bootloader.img");
    BuildSuccess = LinkWithWatcom(BuildContext);

    PopSubTarget(BuildContext);

    return BuildSuccess;
}