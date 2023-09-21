#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <strsafe.h>
#include <fileapi.h>
#include <direct.h>
#include <shlwapi.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\basic_defines.h"
#include "..\miscellaneous\assertions.h"

#include "..\miscellaneous\strings.cpp"

struct directory_node
{
    char DirectoryPath[1024];
    directory_node *NextDirectoryNode;
};

void PushDirectoryNode(char *Path, directory_node **LastNode)
{
    directory_node *NewDirectoryNode = (directory_node *)malloc(sizeof(directory_node));
    *NewDirectoryNode = {};
    StringCchCatA(NewDirectoryNode->DirectoryPath, ArrayLength(NewDirectoryNode->DirectoryPath), Path);
    NewDirectoryNode->NextDirectoryNode = *LastNode;
    *LastNode = NewDirectoryNode;
}

b32 ProcessDirectory(char *DirectoryPath, directory_node **FoundDirectoriesList)
{
    b32 DirectoryContainsSourceCode = FALSE;
    b32 Result = TRUE;

    char DirectoryWildcard[1024];
    ZeroMemory(DirectoryWildcard, ArrayLength(DirectoryWildcard));
    StringCchCatA(DirectoryWildcard, ArrayLength(DirectoryWildcard), DirectoryPath);
    StringCchCatA(DirectoryWildcard, ArrayLength(DirectoryWildcard), "\\*");

    WIN32_FIND_DATAA FindOperationData;
    HANDLE FindHandle = FindFirstFileA(DirectoryWildcard, &FindOperationData);

    if (FindHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            {
                if
                (
                    (strcmp(FindOperationData.cFileName, ".") == 0) ||
                    (strcmp(FindOperationData.cFileName, "..") == 0) ||
                    (strcmp(FindOperationData.cFileName, ".git") == 0) ||
                    (strcmp(FindOperationData.cFileName, ".vscode") == 0)
                )
                {
                    continue;
                }

                char FoundDirectoryPath[1024];
                ZeroMemory(FoundDirectoryPath, 1024);
                StringCchCatA(FoundDirectoryPath, 1024, DirectoryPath);
                StringCchCatA(FoundDirectoryPath, 1024, "\\");
                StringCchCatA(FoundDirectoryPath, 1024, FindOperationData.cFileName);
                Result = ProcessDirectory(FoundDirectoryPath, FoundDirectoriesList) && Result;
            }
            else
            {
                if (!DirectoryContainsSourceCode)
                {
                    char *Extension = PathFindExtensionA(FindOperationData.cFileName);
                    if ((strcmp(Extension, ".cpp") == 0) || (strcmp(Extension, ".h") == 0))
                    {
                        DirectoryContainsSourceCode = TRUE;
                        PushDirectoryNode(DirectoryPath, FoundDirectoriesList);
                    }
                }
            }
        } while (FindNextFileA(FindHandle, &FindOperationData) != 0);

        DWORD LastErrorCode = GetLastError();
        if (LastErrorCode != ERROR_NO_MORE_FILES)
        {
            printf("ERROR: enumerating directories failed.\n");
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

int main(int argc, char **argv)
{
    // TODO: optimize string operations
    char OutputDirectoryPath[1024];
    ZeroMemory(OutputDirectoryPath, ArrayLength(OutputDirectoryPath));
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    char RootDirectoryPath[1024];
    ZeroMemory(RootDirectoryPath, ArrayLength(RootDirectoryPath));
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), OutputDirectoryPath);
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), "\\..");

    char OutputFilePath[1024];
    ZeroMemory(OutputFilePath, ArrayLength(OutputFilePath));
    StringCchCatA(OutputFilePath, ArrayLength(OutputFilePath), RootDirectoryPath);
    StringCchCatA(OutputFilePath, ArrayLength(OutputFilePath), "\\build\\monorepo_metadata.cpp");

    directory_node *FoundDirectoriesList = 0;

    ProcessDirectory(RootDirectoryPath, &FoundDirectoriesList);

    u8 *BufferMemory = (u8 *)VirtualAlloc
    (
        0,
        KiloBytes(5),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    memset(BufferMemory, 0, KiloBytes(5));

    u8 *WritePointer = BufferMemory;

    char TemporaryString[1024];
    ZeroMemory(TemporaryString, ArrayLength(TemporaryString));
    StringCchCatA(TemporaryString, ArrayLength(TemporaryString), "char *DirectoriesWithSourceCode[] =\n");
    StringCchCatA(TemporaryString, ArrayLength(TemporaryString), "{\n");
    memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
    WritePointer += StringLength(TemporaryString);

    while (FoundDirectoriesList != 0)
    {
        ZeroMemory(TemporaryString, ArrayLength(TemporaryString));
        StringCchCatA(TemporaryString, ArrayLength(TemporaryString), "    \"");
        InjectEscapeSlashes(FoundDirectoriesList->DirectoryPath, TemporaryString + StringLength(TemporaryString));
        StringCchCatA(TemporaryString, ArrayLength(TemporaryString), "\",\n");

        memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
        WritePointer += StringLength(TemporaryString);

        FoundDirectoriesList = FoundDirectoriesList->NextDirectoryNode;
    }

    ZeroMemory(TemporaryString, ArrayLength(TemporaryString));
    StringCchCatA(TemporaryString, ArrayLength(TemporaryString), "};");
    memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
    WritePointer += StringLength(TemporaryString);

    u64 OutputSize = WritePointer - BufferMemory;
    u8 *OutputFileMemory = (u8 *)VirtualAlloc
    (
        0,
        OutputSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    memcpy(OutputFileMemory, BufferMemory, OutputSize);

    HANDLE FileHandle = CreateFileA
    (
        OutputFilePath,
        GENERIC_WRITE,
        0,
        0,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    DWORD BytesWritten;
    WriteFile(FileHandle, OutputFileMemory, (u32)OutputSize, &BytesWritten, 0);
    CloseHandle(FileHandle);

    VirtualFree(BufferMemory, 0, MEM_RELEASE);
    VirtualFree(OutputFileMemory, 0, MEM_RELEASE);

    return 0;
}