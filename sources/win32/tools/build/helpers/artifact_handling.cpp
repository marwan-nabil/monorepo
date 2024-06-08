#include <Windows.h>
#include <stdint.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"

#include "artifact_handling.h"

artifact_table_entry *ArtifactTable[256];
artifact_table_entry *ArtifactTableEntriesFreeList;

u32 GetHashOfFileName(char *FilePath, u32 HashValueLimit)
{
    u32 HashValue = 0;
    char *FileName = GetPointerToLastSegmentFromPath(FilePath);
    while (*FileName != NULL)
    {
        HashValue += (u32)*FileName;
        FileName++;
    }
    HashValue &= HashValueLimit;
    return HashValue;
}

void AddArtifact(char *FilePath)
{
    artifact_table_entry *ResultEntry = 0;
    u32 HashValue = GetHashOfFileName(FilePath, ArrayCount(ArtifactTable));

    for
    (
        artifact_table_entry *CurrentEntry = ArtifactTable[HashValue];
        CurrentEntry;
        CurrentEntry = CurrentEntry->NextEntry;
    )
    {
        if (strcmp(FilePath, CurrentEntry->FilePath) == 0)
        {
            return;
        }
    }

    if (ArtifactTableEntriesFreeList)
    {
        ResultEntry = ArtifactTableEntriesFreeList;
        ArtifactTableEntriesFreeList = ArtifactTableEntriesFreeList->NextEntry;
    }
    else
    {
        ResultEntry = (artifact_table_entry *)malloc(sizeof(artifact_table_entry));
    }

    ResultEntry->NextEntry = ArtifactTable[HashValue];
    ArtifactTable[HashValue] = ResultEntry;

    memset(ResultEntry->FilePath, FilePath, ArrayCount(ResultEntry->FilePath));
    ResultEntry->FileExists = ;
    ResultEntry->FileLastWriteTime = ;
}
