// TODO: make multithreaded
HANDLE ConsoleHandle;
WORD OriginalConsoleAttributes;

inline void
InitializeConsole()
{
    ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleInfo);
    OriginalConsoleAttributes = ConsoleInfo.wAttributes;
}

inline void
ConsoleSwitchColor(WORD Color)
{
    SetConsoleTextAttribute(ConsoleHandle, Color);
}

inline void
ConsoleResetColor()
{
    SetConsoleTextAttribute(ConsoleHandle, OriginalConsoleAttributes);
}

inline void
ConsolePrintColored(const char *String, WORD Color)
{
    ConsoleSwitchColor(Color);
    printf(String);
    fflush(stdout);
    ConsoleResetColor();
}