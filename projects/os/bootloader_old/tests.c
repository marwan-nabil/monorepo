void MemoryUtilsTests()
{

}

void StringTests()
{
    char *FarString = "far string";
    char *FarString2 = "aaaaaaa";

    PrintString("\r\n============ formatted print tests ============== \r\n");
    PrintFormatted
    (
        "Formatted %% %c %s %ls\r\n",
        'a', "string", FarString
    );
    PrintFormatted
    (
        "Formatted %d %i %x %p %o %hd %hi %hhu %hhd\r\n",
        1234, -5678, 0xdead, 0xbeef, 012345, (short)27,
        (short)-42, (unsigned char)20, (signed char)-10
    );
    PrintFormatted
    (
        "Formatted %ld %lx %lld %llx\r\n",
        -100000000l, 0xdeadbeeful, 10200300400ll, 0xdeadbeeffeebdaedull
    );

    PrintString("\r\n=========== memory utils tests ================= \r\n");
    PrintFormatted("Test MemoryZeroFar Before: %ls\r\n", FarString);
    MemoryZeroFar(FarString, StringLengthFar(FarString));
    PrintFormatted("Test MemoryZeroFar After: %ls\r\n", FarString);
    MemoryCopyFarToFar(FarString, FarString2, StringLengthFar(FarString2));
    PrintFormatted("Test MemoryCopyFarToFar After: %ls\r\n", FarString);

    PrintString("\r\n============ other string tests ============== \r\n");
    char LocalPathBuffer[PATH_HANDLING_MAX_PATH];
    MemoryZeroNear(LocalPathBuffer, ArrayCount(LocalPathBuffer));

    StringConcatenateFarToNear(LocalPathBuffer, ArrayCount(LocalPathBuffer), "test ");
    StringConcatenateFarToNear(LocalPathBuffer, ArrayCount(LocalPathBuffer), "string");
    PrintFormatted("Test string after concatenation: %s\r\n", LocalPathBuffer);
}

void DiskDriverTests(u8 BootDriveNumber)
{
    PrintString("\r\n=========== disk driver tests ================= \r\n");

    disk_parameters DiskParameters;
    GetDiskDriveParameters(&DiskParameters, BootDriveNumber);

    PrintFormatted("Basic Disk Drive parameters:\r\n");
    PrintFormatted("ID: %hhd\r\n", DiskParameters.Id);
    PrintFormatted("Type: %hhd\r\n", DiskParameters.Type);
    PrintFormatted("Cylinders: %hd\r\n", DiskParameters.Cylinders);
    PrintFormatted("Heads: %hd\r\n", DiskParameters.Heads);
    PrintFormatted("Sectors: %hd\r\n", DiskParameters.Sectors);
    PrintFormatted("\r\n");

    PrintFormatted("LBA to CHS translation examples:\r\n");
    u16 LBA, Cylinder, Head, Sector;
    for (u16 Index = 0; Index < 3; Index++)
    {
        LBA = Index;
        TranslateLbaToChs(&DiskParameters, LBA, &Cylinder, &Head, &Sector);
        PrintFormatted("LBA: %hd    CHS: %hd  %hd  %hd\r\n", LBA, Cylinder, Head, Sector);
        SpinlockSleep(100);
    }
    PrintFormatted("\r\n");

    PrintFormatted("reading from disk:\r\n");
    ReadDiskSectors(&DiskParameters, 0, 1, MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS);

    char OEMName[9];
    MemoryZeroNear(OEMName, ArrayCount(OEMName));
    MemoryCopyFarToNear
    (
        OEMName,
        &((boot_sector *)MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS)->OEMName,
        ArrayCount(OEMName) - 1
    );
    PrintFormatted("BootSector OEM name: %s\r\n", OEMName);

    u16 BootSignature = 0;
    MemoryCopyFarToNear
    (
        &BootSignature,
        &((boot_sector *)MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS)->BootSectorSignature,
        sizeof(u16)
    );
    PrintFormatted("BootSector boot signature: %hx\r\n", BootSignature);
    PrintFormatted("\r\n");

    PrintFormatted("modifying OEM name then writing to disk.\r\n");
    ((boot_sector *)MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS)->OEMName[0] = 'K';
    WriteDiskSectors(&DiskParameters, 0, 1, MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS);

    PrintFormatted("reading the bootsector back from disk.\r\n");
    ReadDiskSectors(&DiskParameters, 0, 1, MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS);

    MemoryZeroNear(OEMName, ArrayCount(OEMName));
    MemoryCopyFarToNear
    (
        OEMName,
        &((boot_sector *)MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS)->OEMName,
        ArrayCount(OEMName) - 1
    );
    PrintFormatted("modified BootSector OEM name: %s\r\n", OEMName);
}

void AllocatorTests(u16 BootDriveNumber)
{
    PrintString("\r\n=========== memory allocator tests ================= \r\n");

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        (memory_arena *)&LocalMemoryArena,
        KiloBytes(10),
        MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS
    );

    typedef struct
    {
        u32 A;
        u32 B;
        u32 C;
    } my_struct;

    my_struct *MyStructInstance = PushStruct((memory_arena *)&LocalMemoryArena, my_struct);
    my_struct *MyStructInstance2 = PushStruct((memory_arena *)&LocalMemoryArena, my_struct);
    my_struct *MyStructInstance3 = PushStruct((memory_arena *)&LocalMemoryArena, my_struct);
    PrintFormatted("Pointer of first object allocated:  0x%lx\r\n", (u32)MyStructInstance);
    PrintFormatted("Pointer of second object allocated: 0x%lx\r\n", (u32)MyStructInstance2);
    PrintFormatted("Pointer of third object allocated:  0x%lx\r\n", (u32)MyStructInstance3);
}

void PathHandlingTests(u16 BootDriveNumber)
{
    PrintString("\r\n=========== path handling tests ================= \r\n");

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        (memory_arena *)&LocalMemoryArena,
        KiloBytes(10),
        MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS
    );

    file_path_node *PathListHead = CreateFilePathSegmentList
    (
        "\\aaa\\bbb\\ccc",
        (memory_arena *)&LocalMemoryArena
    );
    PrintFormatted
    (
        "reconstructed path: %ls %ls %ls\r\n",
        PathListHead->FileName,
        PathListHead->ChildNode->FileName,
        PathListHead->ChildNode->ChildNode->FileName
    );

    char *LocalString = "\\xxx\\yyy\\zzz";
    PrintFormatted("local string before trimming: %ls\r\n", LocalString);
    RemoveLastSegmentFromPath(LocalString);
    PrintFormatted("local string after trimming: %ls\r\n", LocalString);
}

void Fat12Tests(u16 BootDriveNumber)
{
    PrintString("\r\n=========== fat12 tests ================= \r\n");

    disk_parameters DiskParameters;
    GetDiskDriveParameters(&DiskParameters, BootDriveNumber);

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        &LocalMemoryArena,
        KiloBytes(10),
        MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS
    );

    fat12_ram_disk *RamDisk = MEMORY_LAYOUT_FILESYSTEM_DATA_FAR_ADDRESS;
    InitializeFat12RamDisk(&DiskParameters, &LocalMemoryArena, RamDisk);

    Fat12ListDirectory
    (
        RamDisk,
        (memory_arena *)&LocalMemoryArena,
        &DiskParameters,
        "\\"
    );
    Fat12ListDirectory
    (
        RamDisk,
        (memory_arena *)&LocalMemoryArena,
        &DiskParameters,
        "\\Dir0"
    );
    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
        &DiskParameters,
        "\\Dir0\\Dir1"
    );

    directory_entry *FileHandle = GetDirectoryEntryOfFileByPath
    (
        RamDisk, (memory_arena *)&LocalMemoryArena, &DiskParameters, "\\Dir0\\Dir1\\sample.txt"
    );

    PrintFormatted("\r\n");
    PrintFormatted("name of opened file handle: %ls\r\n", FileHandle->FileName);

    PrintFormatted("\r\n");
    PrintFormatted("creating a new file in root directory...\r\n");
    Fat12AddFileByPath(RamDisk, &LocalMemoryArena, &DiskParameters, "\\test.txt", NULL, 30);
    Fat12ListDirectory
    (
        RamDisk,
        (memory_arena *)&LocalMemoryArena,
        &DiskParameters,
        "\\"
    );

    PrintFormatted("\r\n");
    PrintFormatted("create a new file in sub directory.\r\n");
    Fat12AddFileByPath(RamDisk, (memory_arena *)&LocalMemoryArena, &DiskParameters, "\\Dir0\\test.txt", NULL, 400);
    Fat12ListDirectory
    (
        RamDisk,
        (memory_arena *)&LocalMemoryArena,
        &DiskParameters,
        "\\Dir0"
    );

    PrintFormatted("\r\n");
    PrintFormatted("reload the disk to see if file system modifications are persistent.\r\n");
    InitializeFat12RamDisk(&DiskParameters, &LocalMemoryArena, RamDisk);

    Fat12ListDirectory
    (
        RamDisk,
        (memory_arena *)&LocalMemoryArena,
        &DiskParameters,
        "\\"
    );
    Fat12ListDirectory
    (
        RamDisk,
        (memory_arena *)&LocalMemoryArena,
        &DiskParameters,
        "\\Dir0"
    );
    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
        &DiskParameters,
        "\\Dir0\\Dir1"
    );
}

void FileIoTests(u16 BootDriveNumber)
{
    PrintString("\r\n=========== File IO tests ================= \r\n");

    file_io_context *FileIoContext = MEMORY_LAYOUT_FILESYSTEM_DATA_FAR_ADDRESS;
    FileIoInitialize
    (
        BootDriveNumber,
        MEMORY_LAYOUT_FREE_MEMORY_FAR_ADDRESS,
        MEMORY_LAYOUT_FREE_MEMORY_SIZE,
        FileIoContext
    );

    ListDirectory(FileIoContext, "\\");
    ListDirectory(FileIoContext, "\\Dir0");
    ListDirectory(FileIoContext, "\\Dir0\\Dir1");

    i16 FileHandle = FileOpen(FileIoContext, "\\Dir0\\BigFile");

    char LocalStringBuffer[30];
    MemoryZeroNear(LocalStringBuffer, 30);

    u32 FileOffset = 0x4B10;

    FileSeek(FileIoContext, FileHandle, FileOffset);
    FileRead(FileIoContext, FileHandle, 29, (u8 *)LocalStringBuffer);
    PrintFormatted("Data read from the file: %s\r\n", LocalStringBuffer);

    PrintFormatted("overwriting the same data read before with A's ...\r\n");

    MemorySetNear(LocalStringBuffer, 'A', 30);
    FileSeek(FileIoContext, FileHandle, FileOffset);
    FileWrite(FileIoContext, FileHandle, 30, (u8 *)LocalStringBuffer);

    FileSeek(FileIoContext, FileHandle, FileOffset);
    FileRead(FileIoContext, FileHandle, 29, (u8 *)LocalStringBuffer);
    PrintFormatted("Data read from the file: %s\r\n", LocalStringBuffer);
}