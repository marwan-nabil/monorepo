#pragma once

struct console_context
{
    HANDLE ConsoleHandle;
    WORD OriginalConsoleAttributes;
};

void InitializeConsole(console_context *ConsoleContext);
void ConsoleSwitchColor(console_context *ConsoleContext, WORD Color);
void ConsoleResetColor(console_context *ConsoleContext);
void ConsolePrintColored(const char *String, console_context *ConsoleContext, WORD Color);