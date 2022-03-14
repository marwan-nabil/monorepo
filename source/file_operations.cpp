inline void DeleteFileSynchronous(char *FilePath)
{
    DWORD GetTempPathA
    (
        [in]  DWORD nBufferLength,
        [out] LPSTR lpBuffer
    );

    GetTempFileNameA
    (
        [in]  LPCSTR lpPathName,
        [in]  LPCSTR lpPrefixString,
        [in]  UINT   uUnique,
        [out] LPSTR  lpTempFileName
    );

    DeleteFileA(FilePathBuffer);
    // TODO: wait for all shell delete operation to finish
}

inline void
DeleteFilesWithExtentionFromDirectory(const char *Extension, const char *BaseDirectoryPath)
{
    bool32 Error = false;
    char FilePathPattern[MAX_PATH];

    if ((StringLength(BaseDirectoryPath) + StringLength("\\*.") + StringLength(Extension) + 2) >= MAX_PATH)
    {
        printf("ERROR: DeleteFilesWithExtentionFromDirectory(): [BaseDirectoryPath + \\*. + Extension + 2] is over MAX_PATH!\n");
        Error = true;
    }

    if (Error == false)
    {
        CopyString(FilePathPattern, MAX_PATH, BaseDirectoryPath);
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
            CopyString(FilePathBuffer, MAX_PATH, BaseDirectoryPath);
            AppendString(FilePathBuffer, MAX_PATH, "\\");

            if ((FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                AppendString(FilePathBuffer, MAX_PATH, FindOperationData.cFileName);
                DeleteFileSynchronous(FilePathBuffer);
                printf("deleted file: %s\n", FilePathBuffer);
            }
        } while (FindNextFileA(FindHandle, &FindOperationData) != 0);

        DWORD LastError = GetLastError();
        Assert(LastError == ERROR_NO_MORE_FILES);
        FindClose(FindHandle);
    }
}
