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