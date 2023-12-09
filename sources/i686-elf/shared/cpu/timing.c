#include "sources/i686-elf/shared/base_types.h"
#include "sources/i686-elf/shared/basic_defines.h"
#include "sources/i686-elf/shared/cpu/timing.h"

void SpinlockWait(u32 SleepLoops)
{
    SleepLoops *= 1000000;
    for (u32 Index = 0; Index < SleepLoops; Index++) {;}
}