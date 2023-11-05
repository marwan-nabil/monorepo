static b32 BuildFloppyDiskImage(build_context *BuildContext)
{
    fat12_disk *Fat12Disk = (fat12_disk *)VirtualAlloc
    (
        0, sizeof(fat12_disk), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
    );
    memset(Fat12Disk, 0, sizeof(fat12_disk));

    SetOuputBinaryPath(BuildContext, "\\boot_sector\\boot_sector.img");
    read_file_result BootSector = ReadFileIntoMemory(BuildContext->OutputBinaryPath);
    memcpy(&Fat12Disk->BootSector, BootSector.FileMemory, FAT12_SECTOR_SIZE);
    FreeFileMemory(BootSector);
    ClearBuildContext(BuildContext);

    SetOuputBinaryPath(BuildContext, "\\bootloader\\bootloader.img");
    read_file_result Bootloader = ReadFileIntoMemory(BuildContext->OutputBinaryPath);
    Fat12AddFile(Fat12Disk, "\\bootld  .bin", Bootloader.FileMemory, Bootloader.Size);
    FreeFileMemory(Bootloader);
    ClearBuildContext(BuildContext);

    SetOuputBinaryPath(BuildContext, "\\kernel\\kernel.img");
    read_file_result Kernel = ReadFileIntoMemory(BuildContext->OutputBinaryPath);

    Fat12AddFile(Fat12Disk, "\\kernel  .bin", Kernel.FileMemory, Kernel.Size);
    Fat12AddDirectory(Fat12Disk, "\\Dir0");
    Fat12AddFile(Fat12Disk, "\\Dir0\\file0", Kernel.FileMemory, Kernel.Size);

    FreeFileMemory(Kernel);
    ClearBuildContext(BuildContext);

    SetOuputBinaryPath(BuildContext, "\\floppy.img");
    b32 BuildSuccess = WriteFileFromMemory
    (
        BuildContext->OutputBinaryPath,
        Fat12Disk,
        sizeof(fat12_disk)
    );
    VirtualFree(Fat12Disk, 0, MEM_RELEASE);
    return BuildSuccess;
}

static b32 Buildx86Os(build_context *BuildContext)
{
    b32 BuildSuccess = BuildBootSectorImage(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    BuildSuccess = BuildBootloaderImage(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    BuildSuccess = BuildKernelImage(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    BuildSuccess = BuildFloppyDiskImage(BuildContext);
    ClearBuildContext(BuildContext);

    return BuildSuccess;
}