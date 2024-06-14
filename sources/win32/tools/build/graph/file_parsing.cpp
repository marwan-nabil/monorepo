#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"
#include "sources\win32\libraries\shell\console.h"
#include "sources\win32\libraries\file_system\files.h"
#include "sources\win32\libraries\cJSON\cJSON.h"

#include "build_graph.h"
#include "file_parsing.h"

static void ReportParseFailure(char *FilePath)
{
    ConsoleSwitchColor(FOREGROUND_RED);
    printf("ERROR: failed to parse input build description file.\n");
    printf("ERROR:      Error location: %s\n", FilePath);
    ConsoleResetColor();
}

static build_object_type GetBuildObjectTypeFromString(char *TypeString)
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
    else if (strcmp(TypeString, "string_reference_list") == 0)
    {
        Result = BOT_STRING_REFERENCE_LIST;
    }
    else if (strcmp(TypeString, "build_time_condition") == 0)
    {
        Result = BOT_BUILD_TIME_CONDITION;
    }
    return Result;
}

void ParseFileIntoBuildObjects(char *FilePath)
{
    read_file_result File = ReadFileIntoMemory(FilePath);
    cJSON *FileRoot = cJSON_ParseWithLength((char *)File.FileMemory, File.Size);
    if (!FileRoot || !cJSON_IsArray(FileRoot))
    {
        ReportParseFailure(FilePath);
        return;
    }

    u32 ArraySize = cJSON_GetArraySize(FileRoot);
    for (u32 ObjectIndex = 0; ObjectIndex < ArraySize; ObjectIndex++)
    {
        cJSON *CurrentObject = cJSON_GetArrayItem(FileRoot, ObjectIndex);
        if (!cJSON_IsObject(CurrentObject))
        {
            ReportParseFailure(FilePath);
            return;
        }
        
        cJSON *TypeElement = cJSON_GetObjectItem(CurrentObject, "type");
        if (!TypeElement)
        {
            ReportParseFailure(FilePath);
            return;
        }

        build_object_type ElementType = GetBuildObjectTypeFromString(TypeElement->valuestring);
        cJSON *NameElement = cJSON_GetObjectItem(CurrentObject, "name");
        build_object *BuildObject = AddBuildObject(NameElement->valuestring, ElementType);

        // switch (ElementType)
        // {
        //     case BOT_CPP_HEADER_FILE:
        //     {

        //     } break;
        // }
    }

    FreeFileMemory(File);
}