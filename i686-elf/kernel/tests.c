#include <stdarg.h>
#include "i686-elf\libraries\base_types.h"
#include "i686-elf\libraries\basic_defines.h"
#include "i686-elf\libraries\strings\print.h"
#include "i686-elf\libraries\cpu\panic.h"
#include "i686-elf\kernel\main.h"

void TestInterrupts()
{
    PrintString
    (
        &GlobalPrintContext,
        "======================= interrupt tests ================\r\n"
    );
    // __asm("int $0x2");
    // __asm("int $0x3");
    // __asm("int $0x4");
    // __asm("int $50"); // will cause a segmentation exception
    // IntentionalCrash();

    PrintString
    (
        &GlobalPrintContext,
        "Interrupt tests finished.\n"
    );
}