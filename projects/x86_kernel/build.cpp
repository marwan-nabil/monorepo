b32 BuildX86Kernel(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\boot_sector.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\boot_sector.img");

    b32 BuildSuccess = CompileAssembly(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\kernel.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\x86_kernel.img");

    BuildSuccess = CompileAssembly(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    fat12_disk *Fat12Disk = (fat12_disk *)VirtualAlloc
    (
        0, sizeof(fat12_disk), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
    );
    memset(Fat12Disk, 0, sizeof(fat12_disk));

    ClearBuildContext(BuildContext);
    SetOuputBinaryPath(BuildContext, "\\boot_sector.img");
    read_file_result BootSectorImageFile = ReadFileIntoMemory(BuildContext->OutputBinaryPath);
    memcpy(&Fat12Disk->BootSector, BootSectorImageFile.FileMemory, FAT12_SECTOR_SIZE);
    FreeFileMemory(BootSectorImageFile);

    ClearBuildContext(BuildContext);
    SetOuputBinaryPath(BuildContext, "\\x86_kernel.img");
    read_file_result KernelImageFile = ReadFileIntoMemory(BuildContext->OutputBinaryPath);
    Fat12AddFile(Fat12Disk, "\\kernel  .bin", KernelImageFile.FileMemory, KernelImageFile.Size);
    FreeFileMemory(KernelImageFile);

    ClearBuildContext(BuildContext);
    SetOuputBinaryPath(BuildContext, "\\floppy.img");
    BuildSuccess = WriteFileFromMemory
    (
        BuildContext->OutputBinaryPath,
        Fat12Disk,
        sizeof(fat12_disk)
    );
    VirtualFree(Fat12Disk, 0, MEM_RELEASE);

    return BuildSuccess;
}

b32 BuildX86KernelTests(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\test.cpp");

    AddCompilerFlags(BuildContext, "/nologo /I.. /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\x86_kernel_tests.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}