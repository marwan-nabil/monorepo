static b32 BuildProtectedModeBootSectorImage(build_context *BuildContext)
{
    PushSubTarget(BuildContext, "boot_sector");
    AddSourceFile(BuildContext, "\\projects\\low_level\\os_protected\\boot_sector\\entry.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\boot_sector.img");
    SetCompilerIncludePath(BuildContext, BuildContext->RootDirectoryPath);
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    PopSubTarget(BuildContext);
    return BuildSuccess;
}