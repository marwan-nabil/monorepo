void TestVGA()
{
    for (u32 Y = 0; Y < VGA_SCREEN_HEIGHT; Y++)
    {
        for (u32 X = 0; X < VGA_SCREEN_WIDTH; X++)
        {
            u8 Color = X + Y;
            char Character = X + Y;
            PrintCharacterColored(Character, Color, X, Y);
        }
    }
}

void TestBIOSFunctions(u32 DriveNumber)
{
    BIOSPrintCharacter('E');
    BIOSPrintString("xecuting BIOS code!\r\n");

    // TODO: test raw BIOS disk driver calls somehow
#if 0
    u8 DriveType;
    u16 Cylinders;
    u16 Sectors;
    u16 Heads;

    BIOSGetDiskDriveParameters(DriveNumber, &DriveType, &Cylinders, &Sectors, &Heads);
    BIOSPrintString("Drive Type: ");
    BIOSPrintCharacter(DriveType + '0');
    BIOSPrintString(" Cylinders: ");
    BIOSPrintCharacter((u8)Cylinders + '0');
    BIOSPrintString(" Sectors: ");
    BIOSPrintCharacter((u8)Sectors + '0');
    BIOSPrintString(" Heads: ");
    BIOSPrintCharacter((u8)Heads + '0');
    BIOSPrintString("\r\n");
#endif
}

void StringTests()
{
    // TODO: fix bugs in PrintFormatted
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
        1234, -5678, 0xdead, 0xbeef, 012345, (short)27,
        (short)-42, (unsigned char)20, (signed char)-10
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