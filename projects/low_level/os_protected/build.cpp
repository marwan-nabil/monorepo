static b32 BuildProtectedModeOsFloppyImage(build_context *BuildContext)
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

static b32 BuildX86ProtectedOs(build_context *BuildContext)
{
    b32 BuildSuccess = BuildProtectedModeBootSectorImage(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }
    ClearBuildContext(BuildContext);

    BuildSuccess = BuildProtectedModeOsFloppyImage(BuildContext);
    ClearBuildContext(BuildContext);

    return BuildSuccess;
}