b32 CompileShader
(
    char *CompilerFlags,
    char *SourcesString,
    char *OutputBinaryPath
)
{
    char CompilerCommand[1024];
    ZeroMemory(CompilerCommand, ArrayCount(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "fxc.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fo \"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), SourcesString);

    b32 Result = CreateProcessAndWait(CompilerCommand);
    if (!Result)
    {
        ConsolePrintColored("ERROR: shader compilation failed.\n", FOREGROUND_RED);
    }

    return Result;
}

b32 CompileCpp
(
    char *CompilerFlags,
    char *SourcesString,
    char *OutputBinaryPath,
    char *LinkerFlags
)
{
    char CompilerCommand[1024];
    ZeroMemory(CompilerCommand, ArrayCount(CompilerCommand));
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "cl.exe ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), CompilerFlags);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), SourcesString);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), " /Fe:\"");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), OutputBinaryPath);
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "\" ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), "/link ");
    StringCchCatA(CompilerCommand, ArrayCount(CompilerCommand), LinkerFlags);

    b32 Result = CreateProcessAndWait(CompilerCommand);
    if (!Result)
    {
        ConsolePrintColored("ERROR: compilation failed.\n", FOREGROUND_RED);
    }

    return Result;
}

b32 CompileAssembly
(
    char *AssemblerFlags,
    char *SourcesString,
    char *OutputBinaryPath
)
{
    char AssemblerCommand[1024];
    ZeroMemory(AssemblerCommand, ArrayCount(AssemblerCommand));
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "nasm.exe ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), AssemblerFlags);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " ");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), SourcesString);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), " -o \"");
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), OutputBinaryPath);
    StringCchCatA(AssemblerCommand, ArrayCount(AssemblerCommand), "\" ");

    b32 Result = CreateProcessAndWait(AssemblerCommand);
    if (!Result)
    {
        ConsolePrintColored("ERROR: Assembly failed.\n", FOREGROUND_RED);
    }

    return Result;
}

b32 CreateEmptyFile(char *FilePath, u32 Size, u32 FillPattern)
{
    u8 *OutputFileMemory = (u8 *)VirtualAlloc
    (
        0, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
    );
    memset(OutputFileMemory, FillPattern, Size);

    b32 Result = WriteFileFromMemory(FilePath, OutputFileMemory, Size);
    return Result;
}

b32 WriteBinaryFileOverAnother(char *SourceBinaryFilePath, u32 WriteOffset, char *DestinationBinaryFilePath)
{
    b32 Result = FALSE;

    read_file_result SourceBinary = ReadFileIntoMemory(SourceBinaryFilePath);
    read_file_result DestinationBinary = ReadFileIntoMemory(DestinationBinaryFilePath);

    if (DestinationBinary.Size > SourceBinary.Size)
    {
        memcpy(DestinationBinary.FileMemory, SourceBinary.FileMemory, SourceBinary.Size);
        Result = WriteFileFromMemory(DestinationBinaryFilePath, DestinationBinary.FileMemory, DestinationBinary.Size);
    }
    else
    {
        Result = WriteFileFromMemory(DestinationBinaryFilePath, SourceBinary.FileMemory, SourceBinary.Size);
    }

    FreeFileMemory(SourceBinary.FileMemory);
    FreeFileMemory(DestinationBinary.FileMemory);

    return Result;
}