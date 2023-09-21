#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <strsafe.h>
#include <fileapi.h>
#include <direct.h>
#include <time.h>

#include "..\miscellaneous\base_types.h"
#include "..\miscellaneous\basic_defines.h"

#include "monorepo_metadata.cpp"

enum lint_job_type
{
    LJT_FILE_LIST,
    LJT_DIRECTORY
};

struct lint_job
{
    lint_job_type Type;

    union
    {
        struct
        {
            u32 NumberOfFiles;
            char **FilePaths;
        };
        char *DirectoryPath;
    };

    b32 Result;
};

struct lint_job_queue
{
    u32 JobCount;
    lint_job *Jobs;
    volatile i64 NextJobIndex;
    volatile i64 TotalJobsDone;
};

char OutputDirectoryPath[1024];
char RootDirectoryPath[1024];

b32 LintFile(char *FilePath)
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

    if (!FileHandle)
    {
        return FALSE;
    }

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
    b32 Result = ReadFile(FileHandle, ReadFileMemory, (u32)FileSize.QuadPart, &BytesRead, 0);
    if (!Result)
    {
        return FALSE;
    }
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
    if (!FileHandle)
    {
        return FALSE;
    }
    DWORD BytesWritten;
    Result = WriteFile(FileHandle, OutputFileMemory, (u32)OutputSize, &BytesWritten, 0);
    if (!Result)
    {
        return FALSE;
    }
    CloseHandle(FileHandle);

    VirtualFree(ReadFileMemory, 0, MEM_RELEASE);
    VirtualFree(OutputFileMemory, 0, MEM_RELEASE);

    return TRUE;
}

b32 LintDirectoryWithWildcard(char *DirectoryPath, char *FilesWildcard)
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

    b32 Result = LintDirectoryWithWildcard(DirectoryPath, FilesWildcard);

    ZeroMemory(FilesWildcard, ArrayLength(FilesWildcard));
    StringCchCatA(FilesWildcard, MAX_PATH, DirectoryPath);
    StringCchCatA(FilesWildcard, MAX_PATH, "\\*.h");

    Result = Result && LintDirectoryWithWildcard(DirectoryPath, FilesWildcard);

    return Result;
}

void ProcessLintJob(lint_job *LintJob)
{
    LintJob->Result = TRUE;

    if (LintJob->Type == LJT_DIRECTORY)
    {
        char FullDirectoryPath[MAX_PATH];
        ZeroMemory(FullDirectoryPath, ArrayLength(FullDirectoryPath));
        StringCchCatA(FullDirectoryPath, MAX_PATH, LintJob->DirectoryPath);

        LintJob->Result = LintDirectory(FullDirectoryPath);
    }
    else if (LintJob->Type == LJT_FILE_LIST)
    {
        for (u32 FileIndex = 0; FileIndex < LintJob->NumberOfFiles; FileIndex++)
        {
            LintJob->Result = LintFile(LintJob->FilePaths[FileIndex]) && LintJob->Result;
        }
    }
}

DWORD WINAPI
WorkerThreadEntry(void *Parameter)
{
    lint_job_queue *LintJobQueue = (lint_job_queue *)Parameter;

    u64 LintJobIndex = InterlockedExchangeAdd64(&LintJobQueue->NextJobIndex, 1);
    if (LintJobIndex >= LintJobQueue->JobCount)
    {
        return FALSE;
    }

    ProcessLintJob(&LintJobQueue->Jobs[LintJobIndex]);

    InterlockedExchangeAdd64(&LintJobQueue->TotalJobsDone, 1);

    return 0;
}

int main(int argc, char **argv)
{
    ZeroMemory(OutputDirectoryPath, ArrayLength(OutputDirectoryPath));
    _getcwd(OutputDirectoryPath, sizeof(OutputDirectoryPath));

    ZeroMemory(RootDirectoryPath, ArrayLength(RootDirectoryPath));
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), OutputDirectoryPath);
    StringCchCatA(RootDirectoryPath, ArrayLength(RootDirectoryPath), "\\..");

    b32 Result = TRUE;

    clock_t StartTime = clock();

    lint_job_queue LintJobQueue;
    LintJobQueue.JobCount = ArrayLength(DirectoriesWithSourceCode);
    LintJobQueue.NextJobIndex = 0;
    LintJobQueue.TotalJobsDone = 0;
    LintJobQueue.Jobs = (lint_job *)malloc(LintJobQueue.JobCount * sizeof(lint_job));

    for (u32 JobIndex = 0; JobIndex < LintJobQueue.JobCount; JobIndex++)
    {
        lint_job *Job = &LintJobQueue.Jobs[JobIndex];
        ZeroMemory(Job, sizeof(lint_job));

        Job->Type = LJT_DIRECTORY;
        Job->Result = FALSE;
        Job->DirectoryPath = DirectoriesWithSourceCode[JobIndex];
    }


    for (u32 ThreadIndex = 0; ThreadIndex < LintJobQueue.JobCount; ThreadIndex++)
    {
        DWORD ThreadId;
        HANDLE ThreadHandle =
            CreateThread(0, 0, WorkerThreadEntry, &LintJobQueue, 0, &ThreadId);
        CloseHandle(ThreadHandle);
    }

    while (LintJobQueue.TotalJobsDone < LintJobQueue.JobCount) {}

    for (u32 ResultIndex = 0; ResultIndex < LintJobQueue.JobCount; ResultIndex++)
    {
        Result = Result && LintJobQueue.Jobs[ResultIndex].Result;
    }

    printf("\nLinting time: %ld ms\n", clock() - StartTime);

    if (Result)
    {
        printf("\nLint succeeded\n");
        return 0;
    }
    else
    {
        printf("\nLint failed\n");
        return 1;
    }
}