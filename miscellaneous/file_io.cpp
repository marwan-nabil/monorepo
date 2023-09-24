void FreeFileMemory(void *FileMemory)
{
    if (FileMemory)
    {
        VirtualFree(FileMemory, 0, MEM_RELEASE);
    }
}

read_file_result ReadFileIntoMemory(char *FilePath)
{
    read_file_result Result = {};

    HANDLE FileHandle = CreateFileA(FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize))
        {
            void *FileMemory = VirtualAlloc(0, FileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (FileMemory)
            {
                DWORD BytesRead;
                DWORD FileSizeU32 = SafeTruncateUint64ToUint32(FileSize.QuadPart);
                if
                (
                    ReadFile(FileHandle, FileMemory, FileSizeU32, &BytesRead, 0) &&
                    (FileSizeU32 == BytesRead)
                )
                {
                    Result.FileMemory = FileMemory;
                    Result.Size = FileSizeU32;
                }
                else
                {
                    FreeFileMemory(FileMemory);
                }
            }
        }

        CloseHandle(FileHandle);
    }

    return Result;
}

b32 WriteFileFromMemory(char *FilePath, void *DataToWrite, u32 DataSize)
{
    b32 Result = FALSE;

    HANDLE FileHandle = CreateFileA(FilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten;
        if (WriteFile(FileHandle, DataToWrite, DataSize, &BytesWritten, 0))
        {
            Result = (DataSize == BytesWritten);
        }

        CloseHandle(FileHandle);
    }

    return Result;
}