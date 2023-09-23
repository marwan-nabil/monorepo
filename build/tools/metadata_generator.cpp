#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <strsafe.h>
#include <fileapi.h>
#include <direct.h>
#include <shlwapi.h>

#include "..\..\miscellaneous\base_types.h"
#include "..\..\miscellaneous\basic_defines.h"
#include "..\..\miscellaneous\assertions.h"

#include "..\..\miscellaneous\strings.cpp"

// TODO: optimize string operations

struct directory_node
{
    char DirectoryRelativePath[1024];
    directory_node *NextDirectoryNode;
};

struct file_node
{
    char FilePath[1024];
    file_node *NextFileNode;
};

char RootDirectoryPath[1024];

b32 ProcessDirectory(char *DirectoryRelativePath, directory_node **FoundDirectoriesList, file_node **FoundFilesList)
{
    b32 DirectoryContainsSourceCode = FALSE;
    b32 Result = TRUE;

    char DirectoryWildcard[1024];
    ZeroMemory(DirectoryWildcard, ArrayCount(DirectoryWildcard));
    StringCchCatA(DirectoryWildcard, ArrayCount(DirectoryWildcard), RootDirectoryPath);
    StringCchCatA(DirectoryWildcard, ArrayCount(DirectoryWildcard), DirectoryRelativePath);
    StringCchCatA(DirectoryWildcard, ArrayCount(DirectoryWildcard), "\\*");

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

                char FoundDirectoryRelativePath[1024];
                ZeroMemory(FoundDirectoryRelativePath, 1024);
                StringCchCatA(FoundDirectoryRelativePath, 1024, DirectoryRelativePath);
                StringCchCatA(FoundDirectoryRelativePath, 1024, "\\");
                StringCchCatA(FoundDirectoryRelativePath, 1024, FindOperationData.cFileName);
                Result = ProcessDirectory(FoundDirectoryRelativePath, FoundDirectoriesList, FoundFilesList) && Result;
            }
            else
            {
                char *Extension = PathFindExtensionA(FindOperationData.cFileName);
                if ((strcmp(Extension, ".cpp") == 0) || (strcmp(Extension, ".h") == 0) || (strcmp(Extension, ".s") == 0))
                {
                    if (!DirectoryContainsSourceCode)
                    {
                        DirectoryContainsSourceCode = TRUE;
                        directory_node *NewDirectoryNode = (directory_node *)malloc(sizeof(directory_node));
                        *NewDirectoryNode = {};
                        StringCchCatA(NewDirectoryNode->DirectoryRelativePath, ArrayCount(NewDirectoryNode->DirectoryRelativePath), DirectoryRelativePath);
                        NewDirectoryNode->NextDirectoryNode = *FoundDirectoriesList;
                        *FoundDirectoriesList = NewDirectoryNode;
                    }

                    file_node *NewFileNode = (file_node *)malloc(sizeof(file_node));
                    *NewFileNode = {};
                    StringCchCatA(NewFileNode->FilePath, ArrayCount(NewFileNode->FilePath), DirectoryRelativePath);
                    StringCchCatA(NewFileNode->FilePath, ArrayCount(NewFileNode->FilePath), "\\");
                    StringCchCatA(NewFileNode->FilePath, ArrayCount(NewFileNode->FilePath), FindOperationData.cFileName);
                    NewFileNode->NextFileNode = *FoundFilesList;
                    *FoundFilesList = NewFileNode;
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
    char OutputDirectoryPath[1024];
    ZeroMemory(OutputDirectoryPath, ArrayCount(OutputDirectoryPath));
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    ZeroMemory(RootDirectoryPath, ArrayCount(RootDirectoryPath));
    StringCchCatA(RootDirectoryPath, ArrayCount(RootDirectoryPath), OutputDirectoryPath);
    StringCchCatA(RootDirectoryPath, ArrayCount(RootDirectoryPath), "\\..");

    char OutputFilePath[1024];
    ZeroMemory(OutputFilePath, ArrayCount(OutputFilePath));
    StringCchCatA(OutputFilePath, ArrayCount(OutputFilePath), RootDirectoryPath);
    StringCchCatA(OutputFilePath, ArrayCount(OutputFilePath), "\\build\\tools\\metadata.generated.cpp");

    directory_node *FoundDirectoriesList = 0;
    file_node *FoundFilesList = 0;

    ProcessDirectory("", &FoundDirectoriesList, &FoundFilesList);

    u8 *BufferMemory = (u8 *)VirtualAlloc
    (
        0,
        KiloBytes(100),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    memset(BufferMemory, 0, KiloBytes(100));

    u8 *WritePointer = BufferMemory;

    char TemporaryString[1024];
    ZeroMemory(TemporaryString, ArrayCount(TemporaryString));
    StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "char *DirectoriesWithSourceCode[] =\n");
    StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "{\n");
    memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
    WritePointer += StringLength(TemporaryString);

    while (FoundDirectoriesList != 0)
    {
        ZeroMemory(TemporaryString, ArrayCount(TemporaryString));
        StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "    \"");
        InjectEscapeSlashes(FoundDirectoriesList->DirectoryRelativePath, TemporaryString + StringLength(TemporaryString));
        StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "\",\n");

        memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
        WritePointer += StringLength(TemporaryString);

        FoundDirectoriesList = FoundDirectoriesList->NextDirectoryNode;
    }
    ZeroMemory(TemporaryString, ArrayCount(TemporaryString));
    StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "};\n\n");
    memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
    WritePointer += StringLength(TemporaryString);

    ZeroMemory(TemporaryString, ArrayCount(TemporaryString));
    StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "char *FilesWithSourceCode[] =\n");
    StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "{\n");
    memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
    WritePointer += StringLength(TemporaryString);

    while (FoundFilesList != 0)
    {
        ZeroMemory(TemporaryString, ArrayCount(TemporaryString));
        StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "    \"");
        InjectEscapeSlashes(FoundFilesList->FilePath, TemporaryString + StringLength(TemporaryString));
        StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "\",\n");

        memcpy(WritePointer, TemporaryString, StringLength(TemporaryString));
        WritePointer += StringLength(TemporaryString);

        FoundFilesList = FoundFilesList->NextFileNode;
    }

    ZeroMemory(TemporaryString, ArrayCount(TemporaryString));
    StringCchCatA(TemporaryString, ArrayCount(TemporaryString), "};");
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