#include "base_types.h"
#include "x86.h"
#include "strings.h"

void _cdecl cstart(u16 BootDrive)
{
    PutString("Hello World from C\r\n");
    while (1)
    {
    }
}