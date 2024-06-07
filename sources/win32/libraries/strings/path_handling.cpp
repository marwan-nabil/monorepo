#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "strings.h"
#include "path_handling.h"

void
FreeFilePathSegmentList(file_path_node *RootNode)
{
    file_path_node *CurrentNode = RootNode;
    file_path_node *ChildNode;

    while (CurrentNode)
    {
        ChildNode = CurrentNode->ChildNode;
        free(CurrentNode);
        CurrentNode = ChildNode;
    }
}

file_path_node *
CreateFilePathSegmentList(char *FileFullPath)
{
    char LocalPathBuffer[MAX_PATH] = {};
    StringCchCat(LocalPathBuffer, ArrayCount(LocalPathBuffer), FileFullPath);

    u32 PathLength = StringLength(LocalPathBuffer);

    file_path_node *CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
    *CurrentFilePathNode = {};
    file_path_node *LastFilePathNode = 0;

    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (LocalPathBuffer[CharIndex] == '\\')
        {
            char PathSegment[MAX_PATH] = {};
            StringCchCat(PathSegment, MAX_PATH, &LocalPathBuffer[CharIndex + 1]);
            ZeroMemory(&LocalPathBuffer[CharIndex], StringLength(&LocalPathBuffer[CharIndex]));

            if (!CurrentFilePathNode)
            {
                CurrentFilePathNode = (file_path_node *)malloc(sizeof(file_path_node));
                *CurrentFilePathNode = {};
            }

            memcpy(CurrentFilePathNode->FileName, PathSegment, ArrayCount(CurrentFilePathNode->FileName));
            CurrentFilePathNode->ChildNode = LastFilePathNode;

            LastFilePathNode = CurrentFilePathNode;
            CurrentFilePathNode = 0;
        }
    }

    return LastFilePathNode;
}

void RemoveLastSegmentFromPath(char *Path, b8 KeepSlash)
{
    u32 PathLength = StringLength(Path);
    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (Path[CharIndex] == '\\')
        {
            if (KeepSlash)
            {
                ZeroMemory(&Path[CharIndex + 1], StringLength(&Path[CharIndex]));
                return;
            }
            else
            {
                ZeroMemory(&Path[CharIndex], StringLength(&Path[CharIndex]));
                return;
            }
        }
    }
}

char *GetPointerToLastSegmentFromPath(char *Path)
{
    u32 PathLength = StringLength(Path);
    for (i32 CharIndex = PathLength - 1; CharIndex >= 0; CharIndex--)
    {
        if (Path[CharIndex] == '\\')
        {
            return &Path[CharIndex + 1];
        }
    }
    return NULL;
}