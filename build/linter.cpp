#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <strsafe.h>
#include <fileapi.h>
#include <direct.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\basic_defines.h"

// TODO: multi threading

void LintFile(char *FilePath)
{
    HANDLE FileHandle = CreateFile
    (
        FilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        0,
        0
    );

    LARGE_INTEGER FileSize;
    GetFileSizeEx(FileHandle, &FileSize);

    u8 *ReadFileMemory = (u8 *)VirtualAlloc
    (
        0,
        FileSize.QuadPart,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    u8 *BufferMemory = (u8 *)VirtualAlloc
    (
        0,
        FileSize.QuadPart,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    memset(BufferMemory, 0, FileSize.QuadPart);

    DWORD BytesRead;
    ReadFile(FileHandle, ReadFileMemory, (u32)FileSize.QuadPart, &BytesRead, 0);
    CloseHandle(FileHandle);

    u8 *ScanPointer = ReadFileMemory;
    u8 *WritePointer = BufferMemory;

    u8 *CopyStartPointer = ScanPointer;
    u8 *WhiteSpaceRegionPointer = ScanPointer;
    b32 InsideWhiteSpaceRegion = FALSE;

    while (1)
    {
        if (*ScanPointer == ' ')
        {
            if (!InsideWhiteSpaceRegion)
            {
                InsideWhiteSpaceRegion = TRUE;
                WhiteSpaceRegionPointer = ScanPointer;
            }
            ScanPointer++;
        }
        else if
        (
            ((*ScanPointer == '\r') && (*(ScanPointer + 1) == '\n')) ||
            (*ScanPointer == '\0')
        )
        {
            u8 *CopyEndPointer;

            if (InsideWhiteSpaceRegion)
            {
                CopyEndPointer = WhiteSpaceRegionPointer;
            }
            else
            {
                CopyEndPointer = ScanPointer;
            }

            while (CopyStartPointer != CopyEndPointer)
            {
                *WritePointer++ = *CopyStartPointer++;
            }

            if (*ScanPointer == '\0')
            {
                break;
            }
            else
            {
                *WritePointer++ = *ScanPointer++;
                *WritePointer++ = *ScanPointer++;

                CopyStartPointer = ScanPointer;
                WhiteSpaceRegionPointer = ScanPointer;
                InsideWhiteSpaceRegion = FALSE;
            }
        }
        else
        {
            InsideWhiteSpaceRegion = FALSE;
            ScanPointer++;
        }
    }

    WritePointer--;

    while (1)
    {
        if ((*WritePointer == '\n') || (*WritePointer == '\r'))
        {
            WritePointer--;
        }
        else
        {
            WritePointer += 1;
            break;
        }
    }

    u64 OutputSize = WritePointer - BufferMemory;
    u8 *OutputFileMemory = (u8 *)VirtualAlloc
    (
        0,
        OutputSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    memcpy(OutputFileMemory, BufferMemory, OutputSize);

    FileHandle = CreateFileA(FilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    DWORD BytesWritten;
    WriteFile(FileHandle, OutputFileMemory, (u32)OutputSize, &BytesWritten, 0);
    CloseHandle(FileHandle);

    VirtualFree(ReadFileMemory, 0, MEM_RELEASE);
    VirtualFree(OutputFileMemory, 0, MEM_RELEASE);
}

b32 LintFilesWithWildCard(char *DirectoryPath, char *FilesWildcard)
{
    WIN32_FIND_DATAA FindOperationData;
    HANDLE FindHandle = FindFirstFileA(FilesWildcard, &FindOperationData);

    if (FindHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                char FoundFilePath[512];
                ZeroMemory(FoundFilePath, 512);
                StringCchCatA(FoundFilePath, MAX_PATH, DirectoryPath);
                StringCchCatA(FoundFilePath, MAX_PATH, "\\");
                StringCchCatA(FoundFilePath, 512, FindOperationData.cFileName);

                LintFile(FoundFilePath);
            }
        } while (FindNextFileA(FindHandle, &FindOperationData) != 0);

        DWORD LastErrorCode = GetLastError();
        if (LastErrorCode != ERROR_NO_MORE_FILES)
        {
            printf("ERROR: linting process did not finish properly, please debug.\n");
            printf("ERROR: last error code is %d\n", LastErrorCode);
            return FALSE;
        }
    }
    else
    {
        DWORD LastError = GetLastError();
        if (LastError != ERROR_FILE_NOT_FOUND)
        {
            printf("ERROR: FindFirstFileA() failed.\n");
            return FALSE;
        }
    }

    FindClose(FindHandle);

    return TRUE;
}

b32 LintDirectory(char *DirectoryPath)
{
    char FilesWildcard[MAX_PATH];
    ZeroMemory(FilesWildcard, ArrayLength(FilesWildcard));
    StringCchCatA(FilesWildcard, MAX_PATH, DirectoryPath);
    StringCchCatA(FilesWildcard, MAX_PATH, "\\*.cpp");

    b32 Result = LintFilesWithWildCard(DirectoryPath, FilesWildcard);

    ZeroMemory(FilesWildcard, ArrayLength(FilesWildcard));
    StringCchCatA(FilesWildcard, MAX_PATH, DirectoryPath);
    StringCchCatA(FilesWildcard, MAX_PATH, "\\*.h");

    Result = Result && LintFilesWithWildCard(DirectoryPath, FilesWildcard);

    return Result;
}

int main(int argc, char **argv)
{
    char OutputDirectoryPath[1024];
    ZeroMemory(OutputDirectoryPath, ArrayLength(OutputDirectoryPath));
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    char RootDirectoryPath[1024];
    ZeroMemory(RootDirectoryPath, ArrayLength(RootDirectoryPath));
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), OutputDirectoryPath);
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), "\\..");

    char DirectoryToLint[1024];
    ZeroMemory(DirectoryToLint, ArrayLength(DirectoryToLint));

    StringCchCatA(DirectoryToLint, ArrayLength(DirectoryToLint), RootDirectoryPath);
    StringCchCatA(DirectoryToLint, ArrayLength(DirectoryToLint), "\\apps\\handmade_hero");

    LintDirectory(DirectoryToLint);

    return 0;
}