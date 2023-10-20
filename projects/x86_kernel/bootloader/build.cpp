static b32 BuildBootloaderImage(build_context *BuildContext)
{
    char *AssemblySources[]=
    {
        "entry", "x86"
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

        AddCompilerFlags(BuildContext, "-f obj");
        b32 BuildSuccess = AssembleWithNasm(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
        ClearBuildContext(BuildContext);
    }

    char *CSources[]=
    {
        "main", "strings"
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

        AddCompilerFlags(BuildContext, "-4 -d3 -s -ms -zl -zq");
        AddCompilerFlags(BuildContext, "-wx -wcd=138");
        b32 BuildSuccess = CompileWithWatcom(BuildContext);
        if (!BuildSuccess)
        {
            return FALSE;
        }
        ClearBuildContext(BuildContext);
    }

    for (u32 SourceIndex = 0; SourceIndex < ArrayCount(CSources); SourceIndex++)
    {
        char ObjectFileName[1024] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), CSources[SourceIndex]);
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ".obj");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }

    for (u32 SourceIndex = 0; SourceIndex < ArrayCount(AssemblySources); SourceIndex++)
    {
        char ObjectFileName[1024] = {};
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), "\\");
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), AssemblySources[SourceIndex]);
        StringCchCat(ObjectFileName, ArrayCount(ObjectFileName), ".obj");
        AddLinkerInputFile(BuildContext, ObjectFileName);
    }

    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\bootloader\\linker.ls");
    AddLinkerFlags(BuildContext, "OPTION MAP=bootloader.map");
    SetOuputBinaryPath(BuildContext, "\\bootloader.img");
    b32 BuildSuccess = LinkWithWatcom(BuildContext);
    return BuildSuccess;
}