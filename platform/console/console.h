#pragma once

struct console_context
{
    HANDLE ConsoleHandle;
    WORD OriginalConsoleAttributes;
};