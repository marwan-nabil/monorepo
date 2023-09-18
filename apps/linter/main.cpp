#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <fileapi.h>

#include "..\..\miscellaneous\base_types.h"
#include "..\..\miscellaneous\basic_defines.h"

int main(int argc, char **argv)
{
    printf("started linter...\n");

    HANDLE FileHandle = CreateFileA
    (
        argv[1],
        GENERIC_READ | GENERIC_WRITE,
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

    DWORD BytesRead;
    ReadFile(FileHandle, ReadFileMemory, (u32)FileSize.QuadPart, &BytesRead, 0);
    CloseHandle(FileHandle);

    u8 *ScanPointer = ReadFileMemory;
    u8 *WritePointer = BufferMemory;

    while (*ScanPointer != '\0')
    {
        u8 *CopyStartPointer = ScanPointer;
        u8 *WhiteSpaceRegionPointer = ScanPointer;
        b32 InsideWhiteSpaceRegion = FALSE;

        while (*ScanPointer != '\0')
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
            else if (*ScanPointer == '\n')
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

                *WritePointer++ = '\n';
                ScanPointer++;
                break;
            }
            else
            {
                InsideWhiteSpaceRegion = FALSE;
                ScanPointer++;
            }
        }
    }

    *WritePointer++ = (u8)EOF;

    u64 OutputSize = WritePointer - BufferMemory;
    u8 *OutputFileMemory = (u8 *)VirtualAlloc
    (
        0,
        OutputSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    memcpy(OutputFileMemory, BufferMemory, OutputSize);

    FileHandle = CreateFileA("lint_result.c", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    DWORD BytesWritten;
    WriteFile(FileHandle, OutputFileMemory, (u32)FileSize.QuadPart, &BytesWritten, 0);
    CloseHandle(FileHandle);

    VirtualFree(ReadFileMemory, 0, MEM_RELEASE);
    VirtualFree(OutputFileMemory, 0, MEM_RELEASE);
    return 0;
}