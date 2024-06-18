#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"
#include "sources\win32\libraries\shell\console.h"
#include "sources\win32\libraries\file_system\files.h"
#include "sources\win32\libraries\cJSON\cJSON.h"

#include "build_object.h"
#include "file_parsing.h"

static void ReportParseFailure(char *FilePath)
{
    ConsoleSwitchColor(FOREGROUND_RED);
    printf("ERROR: failed to parse input build description file.\n");
    printf("ERROR:      Error location: %s\n", FilePath);
    ConsoleResetColor();
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

        build_object_type ObjectType = GetBuildObjectTypeFromString(TypeElement->valuestring);
        cJSON *NameElement = cJSON_GetObjectItem(CurrentObject, "name");
        build_object *BuildObject = AddBuildObject(NameElement->valuestring, ObjectType);

        switch (ObjectType)
        {
            case BOT_CPP_HEADER_FILE:
            {
                BuildObject->Contents = malloc(sizeof(cpp_header_file));
                cpp_header_file *Contents = (cpp_header_file *)BuildObject->Contents;
                ZeroMemory(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath));
                cJSON *ArtifactPathItem = cJSON_GetObjectItem(CurrentObject, "artifact_path");
                if (!ArtifactPathItem)
                {
                    ReportParseFailure(FilePath);
                    return;
                }
                StringCchCatA(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath), ArtifactPathItem->valuestring);
            } break;

            case BOT_CPP_SOURCE_FILE:
            {
                BuildObject->Contents = malloc(sizeof(cpp_source_file));
                cpp_source_file *Contents = (cpp_source_file *)BuildObject->Contents;
                ZeroMemory(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath));
                cJSON *ArtifactPathItem = cJSON_GetObjectItem(CurrentObject, "artifact_path");
                if (!ArtifactPathItem)
                {
                    ReportParseFailure(FilePath);
                    return;
                }
                StringCchCatA(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath), ArtifactPathItem->valuestring);

                // Contents->HeaderDependenciesCount
                cJSON *HeaderDependenciesElement = cJSON_GetObjectItem(CurrentObject, "header_dependencies");
                if (!HeaderDependenciesElement || !cJSON_IsArray(HeaderDependenciesElement))
                {
                    ReportParseFailure(FilePath);
                    return;
                }
                Contents->HeaderDependenciesCount = cJSON_GetArraySize(HeaderDependenciesElement);

                // Contents->HeaderDependencies
                Contents->HeaderDependencies = (char **)malloc(Contents->HeaderDependenciesCount * sizeof(char *));
                for (u32 HeaderIndex = 0; HeaderIndex < Contents->HeaderDependenciesCount; HeaderIndex++)
                {
                    Contents->HeaderDependencies[HeaderIndex] = (char *)malloc(BUILD_OBJECT_NAME_REFERENCE_LENGTH);
                    ZeroMemory(Contents->HeaderDependencies[HeaderIndex], BUILD_OBJECT_NAME_REFERENCE_LENGTH);
                    cJSON *HeaderDependency = cJSON_GetArrayItem(HeaderDependenciesElement, HeaderIndex);
                    StringCchCatA(Contents->HeaderDependencies[HeaderIndex], BUILD_OBJECT_NAME_REFERENCE_LENGTH, HeaderDependency->valuestring);
                }
            } break;
        }
    }

    FreeFileMemory(File);
}