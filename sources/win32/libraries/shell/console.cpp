#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "console.h"

static console_context ConsoleContext;

void InitializeConsole()
{
    ConsoleContext.ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
    GetConsoleScreenBufferInfo(ConsoleContext.ConsoleHandle, &ConsoleInfo);
    ConsoleContext.OriginalConsoleAttributes = ConsoleInfo.wAttributes;
}

void ConsoleSwitchColor(WORD Color)
{
    SetConsoleTextAttribute(ConsoleContext.ConsoleHandle, Color);
}

void ConsoleResetColor()
{
    SetConsoleTextAttribute(ConsoleContext.ConsoleHandle, ConsoleContext.OriginalConsoleAttributes);
}

void ConsolePrintColored(const char *String, WORD Color)
{
    ConsoleSwitchColor(Color);
    printf(String);
    fflush(stdout);
    ConsoleResetColor();
}