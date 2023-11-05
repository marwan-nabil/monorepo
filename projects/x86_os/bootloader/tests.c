void StringAndMemoryUtilsTests()
{
    char far *FarString = "far string ";
    char far *FarString2 = "aaaaaaa";

    PrintString("\r\n============ string tests ============== \r\n");
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
    PrintFormatted("Test FarMemoryZero Before: %ls\r\n", FarString);
    FarMemoryZero(FarString, StringLengthFar(FarString));
    PrintFormatted("Test FarMemoryZero After: %ls\r\n", FarString);
    FarMemoryCopy(FarString, FarString2, StringLengthFar(FarString2));
    PrintFormatted("Test FarMemoryCopy After: %ls\r\n", FarString);
}

void DiskDriverTests()
{
    PrintString("\r\n=========== disk driver tests ================= \r\n");
    
}

void AllocatorTests()
{
    PrintString("\r\n=========== memory allocator tests ================= \r\n");
    
}

void FileSystemTests()
{
    PrintString("\r\n=========== fat12 filesystem tests ================= \r\n");

}

// void OtherFileSystemTests()
// {
//     PrintString("\r\n=========== disk driver and filesystem tests ================= \r\n");
//     void far *g_data = (void far *)0x00500200;
//     disk_parameters disk;
//     if (!GetDiskDriveParameters(&disk, BootDrive))
//     {
//         PrintFormatted("Disk init error\r\n");
//         goto end;
//     }

//     ReadDiskSectors(&disk, 19, 1, g_data);

//     if (!InitializeFat12FileSystem(&disk))
//     {
//         PrintFormatted("FAT init error\r\n");
//         goto end;
//     }

//     // browse files in root
//     file far *fd = Fat12OpenFile(&disk, "/");
//     directory_entry entry;
//     int i = 0;
//     while (Fat12ReadDirectoryEntry(&disk, fd, &entry) && i++ < 5)
//     {
//         PrintFormatted("  ");
//         for (int i = 0; i < 11; i++)
//             PrintCharacter(entry.Name[i]);
//         PrintFormatted("\r\n");
//     }
//     Fat12CloseFile(fd);

//     // read test.txt
//     char buffer[100];
//     u32 read;
//     fd = Fat12OpenFile(&disk, "mydir/test.txt");
//     while ((read = Fat12ReadFile(&disk, fd, sizeof(buffer), buffer)))
//     {
//         for (u32 i = 0; i < read; i++)
//         {
//             if (buffer[i] == '\n')
//                 PrintCharacter('\r');
//             PrintCharacter(buffer[i]);
//         }
//     }
//     Fat12CloseFile(fd);
// }