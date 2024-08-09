#include "i686-elf\libraries\base_types.h"
#include "i686-elf\libraries\basic_defines.h"

void SpinlockWait(u32 SleepLoops)
{
    SleepLoops *= 1000;
    for (u32 Index = 0; Index < SleepLoops; Index++) {;}
}