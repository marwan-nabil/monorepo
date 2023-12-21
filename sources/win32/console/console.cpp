#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "console.h"

void InitializeConsole(console_context *ConsoleContext)
{
    ConsoleContext->ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
    GetConsoleScreenBufferInfo(ConsoleContext->ConsoleHandle, &ConsoleInfo);
    ConsoleContext->OriginalConsoleAttributes = ConsoleInfo.wAttributes;
}

void ConsoleSwitchColor(console_context *ConsoleContext, WORD Color)
{
    SetConsoleTextAttribute(ConsoleContext->ConsoleHandle, Color);
}

void ConsoleResetColor(console_context *ConsoleContext)
{
    SetConsoleTextAttribute(ConsoleContext->ConsoleHandle, ConsoleContext->OriginalConsoleAttributes);
}

void ConsolePrintColored(const char *String, console_context *ConsoleContext, WORD Color)
{
    ConsoleSwitchColor(ConsoleContext, Color);
    printf(String);
    fflush(stdout);
    ConsoleResetColor(ConsoleContext);
}