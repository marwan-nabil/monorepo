
inline void
DeleteFilesWithExtention(const char *Extension, const char *ParentDirectoryPath)
{
    char FilePathPattern[MAX_PATH];

    CopyString(FilePathPattern, MAX_PATH, ParentDirectoryPath);
    AppendString(FilePathPattern, MAX_PATH, "\\*.");
    AppendString(FilePathPattern, MAX_PATH, Extension);

    FilePathPattern[StringLength(FilePathPattern)] = '\0';
    FilePathPattern[StringLength(FilePathPattern) + 1] = '\0';

    WIN32_FIND_DATAA FindOperationData;
    HANDLE FindHandle = FindFirstFileA(FilePathPattern, &FindOperationData);
    Assert(INVALID_HANDLE_VALUE != FindHandle);
    
    do
    {
        char FilePathBuffer[512];
        ZeroMemory(FilePathBuffer, 512);
        CopyString(FilePathBuffer, MAX_PATH, ParentDirectoryPath);
        AppendString(FilePathBuffer, MAX_PATH, "\\");

        if ((FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            AppendString(FilePathBuffer, MAX_PATH, FindOperationData.cFileName);
            DeleteFileA(FilePathBuffer);
            printf("deleted file: %s\n", FilePathBuffer);
        }
    } while (FindNextFileA(FindHandle, &FindOperationData) != 0);

    DWORD LastError = GetLastError();
    Assert(LastError == ERROR_NO_MORE_FILES);
    FindClose(FindHandle);
}
