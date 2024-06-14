#include <Windows.h>
#include <stdint.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"
#include "sources\win32\libraries\shell\console.h"

#include "build_graph.h"

build_object_table_entry *BuildObjectTable[1024];
build_object_table_entry *BuildObjectTableEntriesFreeList;

static u32 GetHashOfBuildObjectName(char *ObjectName, u32 HashValueLimit)
{
    u32 HashValue = 0;
    while (*ObjectName != NULL)
    {
        HashValue += (u32)*ObjectName;
        ObjectName++;
    }
    HashValue = HashValue % HashValueLimit;
    return HashValue;
}

build_object *GetBuildObject(char *Name)
{
    build_object *Result = NULL;
    u32 HashValue = GetHashOfBuildObjectName(Name, ArrayCount(BuildObjectTable));
    for
    (
        build_object_table_entry *CurrentEntry = BuildObjectTable[HashValue];
        CurrentEntry;
        CurrentEntry = CurrentEntry->NextEntry
    )
    {
        if (strcmp(Name, CurrentEntry->BuildObject.Name) == 0)
        {
            Result = &CurrentEntry->BuildObject;
            break;
        }
    }
    return Result;
}

build_object *AddBuildObject(char *Name, build_object_type Type)
{
    build_object_table_entry *Entry = NULL;
    u32 HashValue = GetHashOfBuildObjectName(Name, ArrayCount(BuildObjectTable));
    for
    (
        build_object_table_entry *CurrentEntry = BuildObjectTable[HashValue];
        CurrentEntry;
        CurrentEntry = CurrentEntry->NextEntry
    )
    {
        if (strcmp(Name, CurrentEntry->BuildObject.Name) == 0)
        {
            ConsolePrintColored
            (
                "ERROR: build graph issue, cannot re-add an already existent build object.\n", 
                FOREGROUND_RED
            );
            return NULL;
        }
    }

    if (BuildObjectTableEntriesFreeList)
    {
        Entry = BuildObjectTableEntriesFreeList;
        BuildObjectTableEntriesFreeList = BuildObjectTableEntriesFreeList->NextEntry;
    }
    else
    {
        Entry = (build_object_table_entry *)malloc(sizeof(build_object_table_entry));
    }

    Entry->NextEntry = BuildObjectTable[HashValue];
    BuildObjectTable[HashValue] = Entry;

    memcpy(Entry->BuildObject.Name, Name, ArrayCount(Entry->BuildObject.Name));
    Entry->BuildObject.Type = Type;

    return &Entry->BuildObject;
}
