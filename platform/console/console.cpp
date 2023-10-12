// TODO_LATER: make multithreaded

inline void
InitializeConsole(console_context *ConsoleContext)
{
    ConsoleContext->ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
    GetConsoleScreenBufferInfo(ConsoleContext->ConsoleHandle, &ConsoleInfo);
    ConsoleContext->OriginalConsoleAttributes = ConsoleInfo.wAttributes;
}

inline void
ConsoleSwitchColor(console_context *ConsoleContext, WORD Color)
{
    SetConsoleTextAttribute(ConsoleContext->ConsoleHandle, Color);
}

inline void
ConsoleResetColor(console_context *ConsoleContext)
{
    SetConsoleTextAttribute(ConsoleContext->ConsoleHandle, ConsoleContext->OriginalConsoleAttributes);
}

inline void
ConsolePrintColored(const char *String, console_context *ConsoleContext, WORD Color)
{
    ConsoleSwitchColor(ConsoleContext, Color);
    printf(String);
    fflush(stdout);
    ConsoleResetColor(ConsoleContext);
}