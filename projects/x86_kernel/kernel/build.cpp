static b32 BuildKernelImage(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\kernel\\entry.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\kernel.img");
    b32 BuildSuccess = AssembleWithNasm(BuildContext);
    return BuildSuccess;
}