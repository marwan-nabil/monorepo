#include <Windows.h>
#include <stdint.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"

#include "artifact_handling.h"

artifact_table_entry *ArtifactTable[256];
artifact_table_entry *ArtifactTableEntriesFreeList;

static u32 GetHashOfFileName(char *FilePath, u32 HashValueLimit)
{
    u32 HashValue = 0;
    char *FileName = GetPointerToLastSegmentFromPath(FilePath);
    while (*FileName != NULL)
    {
        HashValue += (u32)*FileName;
        FileName++;
    }
    HashValue = HashValue % HashValueLimit;
    return HashValue;
}

artifact_table_entry *AddArtifact(char *FilePath)
{
    artifact_table_entry *ArtifactEntry = NULL;
    u32 HashValue = GetHashOfFileName(FilePath, ArrayCount(ArtifactTable));
    for
    (
        artifact_table_entry *CurrentEntry = ArtifactTable[HashValue];
        CurrentEntry;
        CurrentEntry = CurrentEntry->NextEntry
    )
    {
        if (strcmp(FilePath, CurrentEntry->FilePath) == 0)
        {
            ArtifactEntry = CurrentEntry;
        }
    }

    if (!ArtifactEntry)
    {
        if (ArtifactTableEntriesFreeList)
        {
            ArtifactEntry = ArtifactTableEntriesFreeList;
            ArtifactTableEntriesFreeList = ArtifactTableEntriesFreeList->NextEntry;
        }
        else
        {
            ArtifactEntry = (artifact_table_entry *)malloc(sizeof(artifact_table_entry));
        }

        ArtifactEntry->NextEntry = ArtifactTable[HashValue];
        ArtifactTable[HashValue] = ArtifactEntry;

        memcpy(ArtifactEntry->FilePath, FilePath, ArrayCount(ArtifactEntry->FilePath));
        ArtifactEntry->Dependencies = NULL;

        WIN32_FILE_ATTRIBUTE_DATA FileAttributes;
        if (GetFileAttributesEx(FilePath, GetFileExInfoStandard, &FileAttributes))
        {
            ArtifactEntry->FileExists = TRUE;
            ArtifactEntry->FileLastWriteTime = FileAttributes.ftLastWriteTime;
        }
        else
        {
            ArtifactEntry->FileExists = FALSE;
            ArtifactEntry->FileLastWriteTime.dwLowDateTime = 0;
            ArtifactEntry->FileLastWriteTime.dwHighDateTime = 0;
        }
    }

    return ArtifactEntry;
}

void PushArtifactList(artifact_list_node **List, char *FilePath)
{
    artifact_table_entry *Artifact = AddArtifact(FilePath);
    artifact_list_node *NewNode = (artifact_list_node *)malloc(sizeof(artifact_list_node));
    NewNode->Artifact = Artifact;
    NewNode->NextNode = *List;
    *List = NewNode;
}

void FreeArtifactList(artifact_list_node *RootNode)
{
    artifact_list_node *CurrentNode = RootNode;
    artifact_list_node *ChildNode;

    while (CurrentNode)
    {
        ChildNode = CurrentNode->NextNode;
        free(CurrentNode);
        CurrentNode = ChildNode;
    }
}

// artifact_list_node *MakeArtifactList(string_node *ArtifactPathsList, char *RootDirectoryPath)
// {
//     artifact_list_node *Result = NULL;

//     // char AbsoluteFilePath[ArrayCount(Result->)];

//     // for ()
//     // {
//     //     PushArtifactList(&Result, AbsoluteFilePath);
//     // }

//     return Result;
// }
