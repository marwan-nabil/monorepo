#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"
#include "sources\win32\libraries\shell\console.h"

#include "target.h"
#include "target_table.h"

target_table_entry *TargetTable[1024];
target_table_entry *TargetTableEntriesFreeList;

static u32 GetHashOfTargetName(char *TargetName, u32 HashValueLimit)
{
    u32 HashValue = 0;
    while (*TargetName != NULL)
    {
        HashValue += (u32)*TargetName;
        TargetName++;
    }
    HashValue = HashValue % HashValueLimit;
    return HashValue;
}

target_type GetTargetTypeFromString(char *TypeString)
{
    target_type Result = TT_INVALID;
    if (strcmp(TypeString, "cpp_header_file") == 0)
    {
        Result = TT_CPP_HEADER_FILE;
    }
    else if (strcmp(TypeString, "cpp_source_file") == 0)
    {
        Result = TT_CPP_SOURCE_FILE;
    }
    else if (strcmp(TypeString, "win32_executable_file") == 0)
    {
        Result = TT_WIN32_EXECUTABLE_FILE;
    }
    else if (strcmp(TypeString, "msvc_object_file") == 0)
    {
        Result = TT_MSVC_OBJECT_FILE;
    }
    else if (strcmp(TypeString, "string_list") == 0)
    {
        Result = TT_STRING_LIST;
    }
    else if (strcmp(TypeString, "conditional_string") == 0)
    {
        Result = TT_CONDITIONAL_STRING;
    }
    else if (strcmp(TypeString, "build_time_condition") == 0)
    {
        Result = TT_BUILD_TIME_CONDITION;
    }
    return Result;
}

target *GetTarget(char *Name)
{
    target *Result = NULL;
    u32 HashValue = GetHashOfTargetName(Name, ArrayCount(TargetTable));
    for
    (
        target_table_entry *CurrentEntry = TargetTable[HashValue];
        CurrentEntry;
        CurrentEntry = CurrentEntry->NextEntry
    )
    {
        if (strcmp(Name, CurrentEntry->Target.Name) == 0)
        {
            Result = &CurrentEntry->Target;
            break;
        }
    }
    return Result;
}

target *AddTarget(char *Name, char *PackagePath, target_type Type)
{
    if (Type == TT_INVALID)
    {
        return NULL;
    }

    target_table_entry *Entry = NULL;
    u32 HashValue = GetHashOfTargetName(Name, ArrayCount(TargetTable));
    for
    (
        target_table_entry *CurrentEntry = TargetTable[HashValue];
        CurrentEntry;
        CurrentEntry = CurrentEntry->NextEntry
    )
    {
        if (strcmp(Name, CurrentEntry->Target.Name) == 0)
        {
            ConsolePrintColored
            (
                "ERROR: build graph issue, cannot add an already existent build target.\n",
                FOREGROUND_RED
            );
            return NULL;
        }
    }

    if (TargetTableEntriesFreeList)
    {
        Entry = TargetTableEntriesFreeList;
        TargetTableEntriesFreeList = TargetTableEntriesFreeList->NextEntry;
    }
    else
    {
        Entry = (target_table_entry *)malloc(sizeof(target_table_entry));
    }

    ZeroMemory(Entry, sizeof(target_table_entry));
    Entry->NextEntry = TargetTable[HashValue];
    TargetTable[HashValue] = Entry;

    StringCchCatA(Entry->Target.Name, ArrayCount(Entry->Target.Name), Name);
    StringCchCatA(Entry->Target.PackagePath, ArrayCount(Entry->Target.PackagePath), PackagePath);
    Entry->Target.Type = Type;

    return &Entry->Target;
}
