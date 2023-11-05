#include "platforms\x86_real\base_types.h"
#include "platforms\x86_real\basic_defines.h"
#include "platforms\x86_real\strings.h"
#include "platforms\x86_real\integers.h"
#include "platforms\x86_real\disk.h"
#include "platforms\x86_real\fat12.h"

#include "memory_layout.h"

#include "platforms\x86_real\memory.c"
#include "platforms\x86_real\characters.c"
#include "platforms\x86_real\strings.c"
#include "platforms\x86_real\integers.c"
#include "platforms\x86_real\disk.c"
#include "platforms\x86_real\fat12.c"

void far *g_data = (void far *)0x00500200;

void _cdecl cstart(u16 BootDrive)
{
    PrintString("Hello world from C!\r\n");

    char far *FarString = "far string ";
    char far *FarString2 = "aaaaaaa";

    PrintString(" ============ string tests ============== \r\n");
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

    PrintString(" =========== memory utils tests ================= \r\n");
    PrintFormatted("Test FarMemoryZero Before: %ls\r\n", FarString);
    FarMemoryZero(FarString, StringLengthFar(FarString));
    PrintFormatted("Test FarMemoryZero After: %ls\r\n", FarString);
    FarMemoryCopy(FarString, FarString2, StringLengthFar(FarString2));
    PrintFormatted("Test FarMemoryCopy After: %ls\r\n", FarString);

#if 0
    PrintString(" =========== disk driver and filesystem tests ================= \r\n");
    disk_parameters disk;
    if (!GetDiskDriveParameters(&disk, BootDrive))
    {
        PrintFormatted("Disk init error\r\n");
        goto end;
    }

    ReadDiskSectors(&disk, 19, 1, g_data);

    if (!InitializeFat12FileSystem(&disk))
    {
        PrintFormatted("FAT init error\r\n");
        goto end;
    }

    // browse files in root
    file far *fd = Fat12OpenFile(&disk, "/");
    directory_entry entry;
    int i = 0;
    while (Fat12ReadDirectoryEntry(&disk, fd, &entry) && i++ < 5)
    {
        PrintFormatted("  ");
        for (int i = 0; i < 11; i++)
            PrintCharacter(entry.Name[i]);
        PrintFormatted("\r\n");
    }
    Fat12CloseFile(fd);

    // read test.txt
    char buffer[100];
    u32 read;
    fd = Fat12OpenFile(&disk, "mydir/test.txt");
    while ((read = Fat12ReadFile(&disk, fd, sizeof(buffer), buffer)))
    {
        for (u32 i = 0; i < read; i++)
        {
            if (buffer[i] == '\n')
                PrintCharacter('\r');
            PrintCharacter(buffer[i]);
        }
    }
    Fat12CloseFile(fd);

end:
#endif
    while (1) {};
}