void TestVGA()
{
    BIOSPrintString("\r\n============ VGA driver tests ============== \r\n");
    for (u32 Y = 0; Y < VGA_SCREEN_HEIGHT; Y++)
    {
        for (u32 X = 0; X < VGA_SCREEN_WIDTH; X++)
        {
            u8 Color = X + Y;
            char Character = X + Y;
            PrintCharacterColored(Character, Color, X, Y);
            SpinlockSleep(100);
        }
    }
    ClearScreen();
    SetCursorPosition(0, 0);
}

void TestIO()
{
    BIOSPrintString("\r\n============ IO driver tests ============== \r\n");
}

void TestBIOSFunctions(u32 DriveNumber)
{
    BIOSPrintString("\r\n============ BIOS functions tests ============== \r\n");
    BIOSPrintCharacter('E');
    BIOSPrintString("xecuting BIOS code!\r\n");
}

void StringTests()
{
    char *FarString = "far string";
    char *FarString2 = "aaaaaaa";

    BIOSPrintString("\r\n============ formatted print tests ============== \r\n");
    PrintFormatted
    (
        "Formatted %% %c %s %s\r\n",
        'a', "string", FarString
    );
    PrintFormatted
    (
        "Formatted %d %i %x %p %o %hd %hi %hhu %hhd\r\n",
        1234, -5678, 0xdead, 0xbeef, 012345, (i16)27,
        (i16)-42, (u8)20, (i8)-10
    );

    PrintFormatted
    (
        "Formatted %d %x %lld %llx\r\n",
        -100000, 0xdeadbeef, 10200300400ll, 0xdeadbeeffeebdaed
    );

    BIOSPrintString("\r\n=========== memory utils tests ================= \r\n");
    PrintFormatted("Test MemoryZero Before: %s\r\n", FarString);
    MemoryZero(FarString, StringLength(FarString));
    PrintFormatted("Test MemoryZero After: %s\r\n", FarString);
    MemoryCopy(FarString, FarString2, StringLength(FarString2));
    PrintFormatted("Test MemoryCopy After: %s\r\n", FarString);

    BIOSPrintString("\r\n============ other string tests ============== \r\n");
    char LocalPathBuffer[1024];
    MemoryZero(LocalPathBuffer, ArrayCount(LocalPathBuffer));

    StringConcatenate(LocalPathBuffer, ArrayCount(LocalPathBuffer), "test ");
    StringConcatenate(LocalPathBuffer, ArrayCount(LocalPathBuffer), "string");
    PrintFormatted("Test string after concatenation: %s\r\n", LocalPathBuffer);
}

void DiskDriverTests(u8 BootDriveNumber, void *FreeMemoryArea)
{
    BIOSPrintString("\r\n=========== disk driver tests ================= \r\n");

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
    ReadDiskSectors(&DiskParameters, 0, 1, FreeMemoryArea);

    char OEMName[9];
    MemoryZero(OEMName, ArrayCount(OEMName));
    MemoryCopy
    (
        OEMName,
        &((boot_sector *)FreeMemoryArea)->OEMName,
        ArrayCount(OEMName) - 1
    );
    PrintFormatted("BootSector OEM name: %s\r\n", OEMName);

    u16 BootSignature = 0;
    MemoryCopy
    (
        &BootSignature,
        &((boot_sector *)FreeMemoryArea)->BootSectorSignature,
        sizeof(u16)
    );
    PrintFormatted("BootSector boot signature: %hx\r\n", BootSignature);
    PrintFormatted("\r\n");

    PrintFormatted("modifying OEM name then writing to disk.\r\n");
    ((boot_sector *)FreeMemoryArea)->OEMName[0] = 'K';
    WriteDiskSectors(&DiskParameters, 0, 1, FreeMemoryArea);

    PrintFormatted("reading the bootsector back from disk.\r\n");
    ReadDiskSectors(&DiskParameters, 0, 1, FreeMemoryArea);

    MemoryZero(OEMName, ArrayCount(OEMName));
    MemoryCopy
    (
        OEMName,
        &((boot_sector *)FreeMemoryArea)->OEMName,
        ArrayCount(OEMName) - 1
    );
    PrintFormatted("modified BootSector OEM name: %s\r\n", OEMName);
}

void AllocatorTests(void *FreeMemoryAddress)
{
    BIOSPrintString("\r\n=========== memory allocator tests ================= \r\n");

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        &LocalMemoryArena,
        KiloBytes(10),
        FreeMemoryAddress
    );

    typedef struct
    {
        u32 A;
        u32 B;
        u32 C;
    } my_struct;

    my_struct *MyStructInstance = PushStruct(&LocalMemoryArena, my_struct);
    my_struct *MyStructInstance2 = PushStruct(&LocalMemoryArena, my_struct);
    my_struct *MyStructInstance3 = PushStruct(&LocalMemoryArena, my_struct);
    PrintFormatted("Pointer of first object allocated:  0x%lx\r\n", (u32)MyStructInstance);
    PrintFormatted("Pointer of second object allocated: 0x%lx\r\n", (u32)MyStructInstance2);
    PrintFormatted("Pointer of third object allocated:  0x%lx\r\n", (u32)MyStructInstance3);
}

void PathHandlingTests(void *FreeMemoryArea)
{
    BIOSPrintString("\r\n=========== path handling tests ================= \r\n");

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        &LocalMemoryArena,
        KiloBytes(10),
        FreeMemoryArea
    );

    file_path_node *PathListHead = CreateFilePathSegmentList
    (
        "\\aaa\\bbb\\ccc",
        &LocalMemoryArena
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

void Fat12Tests(u16 BootDriveNumber, void *FreeMemoryArea)
{
    BIOSPrintString("\r\n=========== fat12 tests ================= \r\n");

    disk_parameters DiskParameters;
    GetDiskDriveParameters(&DiskParameters, BootDriveNumber);

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        &LocalMemoryArena,
        KiloBytes(10),
        FreeMemoryArea
    );

    fat12_ram_disk *RamDisk = PushStruct(&LocalMemoryArena, fat12_ram_disk);
    InitializeFat12RamDisk(&DiskParameters, &LocalMemoryArena, RamDisk);

    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
        &DiskParameters,
        "\\"
    );
    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
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
        RamDisk, &LocalMemoryArena, &DiskParameters, "\\Dir0\\Dir1\\sample.txt"
    );

    PrintFormatted("\r\n");
    PrintFormatted("name of opened file handle: %ls\r\n", FileHandle->FileName);

    PrintFormatted("\r\n");
    PrintFormatted("creating a new file in root directory...\r\n");
    Fat12AddFileByPath(RamDisk, &LocalMemoryArena, &DiskParameters, "\\test.txt", NULL, 30);
    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
        &DiskParameters,
        "\\"
    );

    PrintFormatted("\r\n");
    PrintFormatted("create a new file in sub directory.\r\n");
    Fat12AddFileByPath(RamDisk, &LocalMemoryArena, &DiskParameters, "\\Dir0\\test.txt", NULL, 400);
    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
        &DiskParameters,
        "\\Dir0"
    );

    PrintFormatted("\r\n");
    PrintFormatted("reload the disk to see if file system modifications are persistent.\r\n");
    InitializeFat12RamDisk(&DiskParameters, &LocalMemoryArena, RamDisk);

    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
        &DiskParameters,
        "\\"
    );
    Fat12ListDirectory
    (
        RamDisk,
        &LocalMemoryArena,
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

void FileIoTests(u16 BootDriveNumber, void *FreeMemoryArea)
{
    BIOSPrintString("\r\n=========== File IO tests ================= \r\n");

    memory_arena LocalMemoryArena;
    InitializeMemoryArena
    (
        &LocalMemoryArena,
        KiloBytes(64),
        FreeMemoryArea
    );

    file_io_context *FileIoContext = PushStruct(&LocalMemoryArena, file_io_context);
    FileIoInitialize
    (
        BootDriveNumber,
        PushArray(&LocalMemoryArena, KiloBytes(16), u8),
        KiloBytes(16),
        FileIoContext
    );

    ListDirectory(FileIoContext, "\\");
    ListDirectory(FileIoContext, "\\Dir0");
    ListDirectory(FileIoContext, "\\Dir0\\Dir1");

    i16 FileHandle = FileOpen(FileIoContext, "\\Dir0\\Dir1\\sample.txt");

    char LocalStringBuffer[30];
    MemoryZero(LocalStringBuffer, 30);

    u32 FileOffset = 0;

    FileSeek(FileIoContext, FileHandle, FileOffset);
    FileRead(FileIoContext, FileHandle, 29, (u8 *)LocalStringBuffer);
    PrintFormatted("Data read from the file: %s\r\n", LocalStringBuffer);

    PrintFormatted("overwriting the same data read before with A's ...\r\n");

    MemorySet(LocalStringBuffer, 'A', 29);
    FileSeek(FileIoContext, FileHandle, FileOffset);
    FileWrite(FileIoContext, FileHandle, 30, (u8 *)LocalStringBuffer);

    FileSeek(FileIoContext, FileHandle, FileOffset);
    FileRead(FileIoContext, FileHandle, 30, (u8 *)LocalStringBuffer);
    PrintFormatted("Data read from the file: %s\r\n", LocalStringBuffer);
}