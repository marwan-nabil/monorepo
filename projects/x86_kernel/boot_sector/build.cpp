static b32 BuildBootSectorImage(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\boot_sector\\entry.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\boot_sector.img");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    return BuildSuccess;
}