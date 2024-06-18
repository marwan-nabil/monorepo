#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"
#include "sources\win32\libraries\shell\console.h"

#include "build_object.h"

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

build_object_type GetBuildObjectTypeFromString(char *TypeString)
{
    build_object_type Result = BOT_INVALID;
    if (strcmp(TypeString, "cpp_header_file") == 0)
    {
        Result = BOT_CPP_HEADER_FILE;
    }
    else if (strcmp(TypeString, "cpp_source_file") == 0)
    {
        Result = BOT_CPP_SOURCE_FILE;
    }
    else if (strcmp(TypeString, "win32_executable_file") == 0)
    {
        Result = BOT_WIN32_EXECUTABLE_FILE;
    }
    else if (strcmp(TypeString, "msvc_object_file") == 0)
    {
        Result = BOT_MSVC_OBJECT_FILE;
    }
    else if (strcmp(TypeString, "string_list") == 0)
    {
        Result = BOT_STRING_LIST;
    }
    else if (strcmp(TypeString, "conditional_string") == 0)
    {
        Result = BOT_CONDITIONAL_STRING;
    }
    else if (strcmp(TypeString, "build_time_condition") == 0)
    {
        Result = BOT_BUILD_TIME_CONDITION;
    }
    return Result;
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

build_object *AddBuildObject(char *Name, build_object_type Type, char *PackagePath)
{
    if (Type == BOT_INVALID)
    {
        return NULL;
    }

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

    ZeroMemory(Entry, sizeof(build_object_table_entry));
    Entry->NextEntry = BuildObjectTable[HashValue];
    BuildObjectTable[HashValue] = Entry;

    StringCchCatA(Entry->BuildObject.Name, ArrayCount(Entry->BuildObject.Name), Name);
    StringCchCatA(Entry->BuildObject.PackagePath, ArrayCount(Entry->BuildObject.PackagePath), PackagePath);
    Entry->BuildObject.Type = Type;

    return &Entry->BuildObject;
}
