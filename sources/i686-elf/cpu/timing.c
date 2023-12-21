#include "sources/i686-elf/base_types.h"
#include "sources/i686-elf/basic_defines.h"

void SpinlockWait(u32 SleepLoops)
{
    SleepLoops *= 1000000;
    for (u32 Index = 0; Index < SleepLoops; Index++) {;}
}