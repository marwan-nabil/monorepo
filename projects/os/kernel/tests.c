#include <stdarg.h>
#include "libraries\i686-elf\base_types.h"
#include "libraries\i686-elf\basic_defines.h"
#include "libraries\i686-elf\strings\print.h"
#include "libraries\i686-elf\cpu\panic.h"

#include "projects\os\kernel\main.h"

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