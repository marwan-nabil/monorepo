#pragma once

struct console_context
{
    HANDLE ConsoleHandle;
    WORD OriginalConsoleAttributes;
};

void InitializeConsole();
void ConsoleSwitchColor(WORD Color);
void ConsoleResetColor();
void ConsolePrintColored(const char *String, WORD Color);