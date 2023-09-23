#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <strsafe.h>
#include <fileapi.h>
#include <direct.h>
#include <time.h>

#include "..\..\miscellaneous\base_types.h"
#include "..\..\miscellaneous\assertions.h"
#include "..\..\miscellaneous\basic_defines.h"

#include "metadata.generated.cpp"

#if !defined(JOB_PER_DIRECTORY) && !defined(JOB_PER_FILE)
#   define JOB_PER_FILE
#endif

enum lint_job_type
{
    LJT_FILE_LIST,
    LJT_DIRECTORY,
    LJT_FILE
};

struct lint_job
{
    lint_job_type Type;

    union
    {
        struct
        {
            u32 NumberOfFiles;
            char **FileRelativePaths;
        };
        char *DirectoryRelativePath;
        char *FileRelativePath;
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

char RootDirectoryPath[1024];

b32 LintFile(char *FileRelativePath)
{
    char FilePath[MAX_PATH];
    ZeroMemory(FilePath, MAX_PATH);
    StringCchCatA(FilePath, MAX_PATH, RootDirectoryPath);
    StringCchCatA(FilePath, MAX_PATH, FileRelativePath);

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

b32 LintDirectoryWithWildcard(char *DirectoryRelativePath, char *FilesWildcard)
{
    char SearchPattern[1024];
    ZeroMemory(SearchPattern, 1024);
    StringCchCatA(SearchPattern, 1024, RootDirectoryPath);
    StringCchCatA(SearchPattern, 1024, DirectoryRelativePath);
    StringCchCatA(SearchPattern, 1024, FilesWildcard);

    WIN32_FIND_DATAA FindOperationData;
    HANDLE FindHandle = FindFirstFileA(SearchPattern, &FindOperationData);

    if (FindHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((FindOperationData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                char FoundFilePath[MAX_PATH];
                ZeroMemory(FoundFilePath, MAX_PATH);
                StringCchCatA(FoundFilePath, MAX_PATH, DirectoryRelativePath);
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

b32 LintDirectory(char *DirectoryRelativePath)
{
    char FilesWildcard[MAX_PATH];
    ZeroMemory(FilesWildcard, ArrayCount(FilesWildcard));
    StringCchCatA(FilesWildcard, MAX_PATH, "\\*.cpp");

    b32 Result = LintDirectoryWithWildcard(DirectoryRelativePath, FilesWildcard);

    ZeroMemory(FilesWildcard, ArrayCount(FilesWildcard));
    StringCchCatA(FilesWildcard, MAX_PATH, "\\*.h");

    Result = LintDirectoryWithWildcard(DirectoryRelativePath, FilesWildcard) && Result;

    return Result;
}

void ProcessLintJob(lint_job *LintJob)
{
    LintJob->Result = TRUE;

    if (LintJob->Type == LJT_DIRECTORY)
    {
        LintJob->Result = LintDirectory(LintJob->DirectoryRelativePath);
    }
    else if (LintJob->Type == LJT_FILE_LIST)
    {
        for (u32 FileIndex = 0; FileIndex < LintJob->NumberOfFiles; FileIndex++)
        {
            LintJob->Result = LintFile(LintJob->FileRelativePaths[FileIndex]) && LintJob->Result;
        }
    }
    else if (LintJob->Type == LJT_FILE)
    {
        LintJob->Result = LintFile(LintJob->FileRelativePath);
    }
}

DWORD WINAPI
WorkerThreadEntry(void *Parameter)
{
    lint_job_queue *LintJobQueue = (lint_job_queue *)Parameter;

    while (LintJobQueue->TotalJobsDone < LintJobQueue->JobCount)
    {
        u64 LintJobIndex = InterlockedExchangeAdd64(&LintJobQueue->NextJobIndex, 1);
        if (LintJobIndex >= LintJobQueue->JobCount)
        {
            return TRUE;
        }

        ProcessLintJob(&LintJobQueue->Jobs[LintJobIndex]);

        InterlockedExchangeAdd64(&LintJobQueue->TotalJobsDone, 1);
    }

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

    b32 Result = TRUE;

    clock_t StartTime = clock();

    lint_job_queue LintJobQueue;
#if defined(JOB_PER_DIRECTORY)
    LintJobQueue.JobCount = ArrayCount(DirectoriesWithSourceCode);
#endif
#if defined(JOB_PER_FILE)
    LintJobQueue.JobCount = ArrayCount(FilesWithSourceCode);
#endif
    LintJobQueue.NextJobIndex = 0;
    LintJobQueue.TotalJobsDone = 0;
    LintJobQueue.Jobs = (lint_job *)malloc(LintJobQueue.JobCount * sizeof(lint_job));

    for (u32 JobIndex = 0; JobIndex < LintJobQueue.JobCount; JobIndex++)
    {
        lint_job *Job = &LintJobQueue.Jobs[JobIndex];
        ZeroMemory(Job, sizeof(lint_job));
#if defined(JOB_PER_DIRECTORY)
        Job->Type = LJT_DIRECTORY;
        Job->DirectoryRelativePath = DirectoriesWithSourceCode[JobIndex];
#endif
#if defined(JOB_PER_FILE)
        Job->Type = LJT_FILE;
        Job->FileRelativePath = FilesWithSourceCode[JobIndex];
#endif
        Job->Result = FALSE;
    }

#if defined(JOB_PER_DIRECTORY)
    u32 ThreadCount = LintJobQueue.JobCount;
#endif
#if defined(JOB_PER_FILE)
    u32 ThreadCount = LintJobQueue.JobCount / 4;
#endif

    printf("\nCreated %d threads.\n", ThreadCount);

    for (u32 ThreadIndex = 0; ThreadIndex < ThreadCount; ThreadIndex++)
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