#include "platforms\x86_real\base_types.h"
#include "platforms\shared\basic_defines.h"

#include "platforms\x86_real\strings.h"
#include "platforms\x86_real\integers.h"
#include "platforms\x86_real\disk.h"

#include "platforms\x86_real\memory.c"
#include "platforms\x86_real\strings.c"
#include "platforms\x86_real\disk.c"

void _cdecl cstart(u16 bootDrive)
{
    char far *FarString = "far string ";
    char far *FarString2 = "aaaaaaa";

    PrintString("Hello world from C!\r\n");
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

    PrintString(" ======================================= \r\n");

    PrintFormatted("Test FarMemoryZero Before: %ls\r\n", FarString);
    FarMemoryZero(FarString, StringLengthFar(FarString));
    PrintFormatted("Test FarMemoryZero After: %ls\r\n", FarString);

    FarMemoryCopy(FarString, FarString2, StringLengthFar(FarString2));
    PrintFormatted("Test FarMemoryCopy After: %ls\r\n", FarString);

    while (1) {};
}