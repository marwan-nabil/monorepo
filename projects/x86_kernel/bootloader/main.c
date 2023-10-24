#include "platforms\x86\base_types.h"
#include "platforms\shared\basic_defines.h"

#include "x86.h"
#include "strings.h"

#include "strings.c"

void _cdecl cstart(u16 BootDrive)
{
    const char far *FarString = "far string";

    // PrintString("Hello world from C!\r\n");
    PrintFormatted("Formatted %% %c %s %ls\r\n", 'a', "string", FarString);
    PrintFormatted
    (
        "Formatted %u \r\n",
        (u32)1234
    );
    // PrintFormatted
    // (
    //     "Formatted %d %i %x %p %o %hd %hi %hhu %hhd\r\n",
    //     1234, -5678, 0xdead, 0xbeef, 012345, (u16)27, (u16)-42, (u8)20, (i8)-10
    // );
    // PrintFormatted
    // (
    //     "Formatted %ld %lx %lld %llx\r\n",
    //     -100000000l, 0xdeadbeeful, 10200300400ll, 0xdeadbeeffeebdaedull
    // );
    while (1)
    {
    }
}