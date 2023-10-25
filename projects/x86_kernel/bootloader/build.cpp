static b32 BuildBootloaderImage(build_context *BuildContext)
{
    char *AssemblySources[]=
    {
        "x86"
    };

    for (u32 SourceIndex = 0; SourceIndex < ArrayCount(AssemblySources); SourceIndex++)
    {
        char SourceFilePath[1024] = {};
        StringCchCat(SourceFilePath, ArrayCount(SourceFilePath), "\\projects\\x86_kernel\\bootloader\\");
        StringCchCat(SourceFilePath, ArrayCount(SourceFilePath), AssemblySources[SourceIndex]);
        StringCchCat(SourceFilePath, ArrayCount(SourceFilePath), ".s");
        AddSourceFile(BuildContext, SourceFilePath);

        char ObjectFileName[1024] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), AssemblySources[SourceIndex]);
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ".obj");
        SetOuputBinaryPath(BuildContext, ObjectFileName);

        AddCompilerFlags(BuildContext, "-i ..\\.. -f obj");
        b32 BuildSuccess = AssembleWithNasm(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
        ClearBuildContext(BuildContext);
    }

    char *CSources[]=
    {
        "main"
    };

    for (u32 SourceIndex = 0; SourceIndex < ArrayCount(CSources); SourceIndex++)
    {
        char SourceFilePath[1024] = {};
        StringCchCat(SourceFilePath, ArrayCount(SourceFilePath), "\\projects\\x86_kernel\\bootloader\\");
        StringCchCat(SourceFilePath, ArrayCount(SourceFilePath), CSources[SourceIndex]);
        StringCchCat(SourceFilePath, ArrayCount(SourceFilePath), ".c");
        AddSourceFile(BuildContext, SourceFilePath);

        char ObjectFileName[1024] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), CSources[SourceIndex]);
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ".obj");
        SetOuputBinaryPath(BuildContext, ObjectFileName);

        AddCompilerFlags(BuildContext, "-4 -d3 -s -ms -zl -zq -i=..\\..");
        AddCompilerFlags(BuildContext, "-wx -wcd=138 -wcd=202");
        b32 BuildSuccess = CompileWithWatcom(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
        ClearBuildContext(BuildContext);
    }

    // TODO: loop over and add all the object files in the directory we're building in
    //       instead of keeping a list of all the stems of the sources c & assembly
    for (u32 SourceIndex = 0; SourceIndex < ArrayCount(AssemblySources); SourceIndex++)
    {
        char ObjectFileName[1024] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), AssemblySources[SourceIndex]);
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ".obj");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }

    for (u32 SourceIndex = 0; SourceIndex < ArrayCount(CSources); SourceIndex++)
    {
        char ObjectFileName[1024] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), CSources[SourceIndex]);
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ".obj");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }

    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\bootloader\\linker.ls");
    SetOuputBinaryPath(BuildContext, "\\bootloader.img");
    b32 BuildSuccess = LinkWithWatcom(BuildContext);
    return BuildSuccess;
}