b32 BuildX86Kernel(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\boot_sector.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\boot_sector.img");

    b32 BuildSuccess = CompileAssembly(BuildContext);

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\kernel.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\x86_kernel.img");

    BuildSuccess = BuildSuccess && CompileAssembly(BuildContext);

    ClearBuildContext(BuildContext);

    fat12_disk *Fat12Disk = Fat12CreateRamDisk();
    SetOuputBinaryPath(BuildContext, "\\x86_kernel.img");
    read_file_result KernelImageFile = ReadFileIntoMemory(BuildContext->OutputBinaryPath);
    AddFileToRootDirectory(Fat12Disk, KernelImageFile.FileMemory, KernelImageFile.Size, "kernel", "bin");
    FreeFileMemory(KernelImageFile.FileMemory);

    ClearBuildContext(BuildContext);

    SetOuputBinaryPath(BuildContext, "\\floppy.img");

    WriteFileFromMemory(BuildContext->OutputBinaryPath, Fat12Disk, sizeof(fat12_disk));
    FreeFileMemory(Fat12Disk);

    char SourceBinaryFilePath[1024] = {};
    StringCchCatA(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath), BuildContext->OutputDirectoryPath);
    StringCchCatA(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath), "\\boot_sector.img");

    BuildSuccess = BuildSuccess && WriteBinaryFileOverAnother(BuildContext->OutputBinaryPath, SourceBinaryFilePath, 0);
    return BuildSuccess;
}

b32 BuildX86KernelTests(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\test.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\x86_kernel_tests.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}