#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"
#include "sources\win32\libraries\shell\console.h"
#include "sources\win32\libraries\file_system\files.h"
#include "sources\win32\libraries\cJSON\cJSON.h"

#include "target.h"
#include "loading.h"

static void ReportParseFailure(char *FilePath)
{
    ConsoleSwitchColor(FOREGROUND_RED);
    printf("ERROR: failed to parse input build description file.\n");
    printf("ERROR:      Error location: %s\n", FilePath);
    ConsoleResetColor();
}

b32 LoadCppHeaderTarget(target *Target, cJSON *JsonObject)
{
    cpp_header_file *Details = (cpp_header_file *)malloc(sizeof(cpp_header_file));
    Target->Details = Details;
    ZeroMemory(Details->ArtifactPath, ArrayCount(Details->ArtifactPath));
    cJSON *ArtifactPathItem = cJSON_GetObjectItem(JsonObject, "artifact_path");
    if (!ArtifactPathItem)
    {
        return FALSE;
    }
    StringCchCatA(Details->ArtifactPath, ArrayCount(Details->ArtifactPath), ArtifactPathItem->valuestring);
    return TRUE;
}

b32 LoadCppSourceTarget(target *Target, cJSON *JsonObject)
{
    cpp_source_file *Details = (cpp_source_file *)malloc(sizeof(cpp_source_file));
    Target->Details = Details;
    ZeroMemory(Details->ArtifactPath, ArrayCount(Details->ArtifactPath));
    cJSON *ArtifactPathItem = cJSON_GetObjectItem(JsonObject, "artifact_path");
    if (!ArtifactPathItem)
    {
        return FALSE;
    }
    StringCchCatA(Details->ArtifactPath, ArrayCount(Details->ArtifactPath), ArtifactPathItem->valuestring);

    // Details->HeaderDependenciesCount
    cJSON *HeaderDependenciesElement = cJSON_GetObjectItem(JsonObject, "header_dependencies");
    if (!HeaderDependenciesElement || !cJSON_IsArray(HeaderDependenciesElement))
    {
        return FALSE;
    }
    Details->HeaderDependenciesCount = cJSON_GetArraySize(HeaderDependenciesElement);

    // Details->HeaderDependencies
    Details->HeaderDependencies = (char **)malloc(Details->HeaderDependenciesCount * sizeof(char *));
    for (u32 HeaderIndex = 0; HeaderIndex < Details->HeaderDependenciesCount; HeaderIndex++)
    {
        Details->HeaderDependencies[HeaderIndex] = (char *)malloc(TARGET_FULL_PATH_BUFFER_SIZE);
        ZeroMemory(Details->HeaderDependencies[HeaderIndex], TARGET_FULL_PATH_BUFFER_SIZE);
        cJSON *HeaderDependency = cJSON_GetArrayItem(HeaderDependenciesElement, HeaderIndex);
        StringCchCatA(Details->HeaderDependencies[HeaderIndex], TARGET_FULL_PATH_BUFFER_SIZE, HeaderDependency->valuestring);
    }
    return TRUE;
}

b32 LoadMsvcObjectFileTarget(target *Target, cJSON *JsonObject)
{
    msvc_object_file *Details = (msvc_object_file *)malloc(sizeof(msvc_object_file));
    Target->Details = Details;
    ZeroMemory(Details->ArtifactPath, ArrayCount(Details->ArtifactPath));
    cJSON *ArtifactPathItem = cJSON_GetObjectItem(JsonObject, "artifact_path");
    if (!ArtifactPathItem)
    {
        return FALSE;
    }
    StringCchCatA(Details->ArtifactPath, ArrayCount(Details->ArtifactPath), ArtifactPathItem->valuestring);

    // Details->SourceDependenciesCount
    cJSON *SourceDependenciesElement = cJSON_GetObjectItem(JsonObject, "source_dependencies");
    if (!SourceDependenciesElement || !cJSON_IsArray(SourceDependenciesElement))
    {
        return FALSE;
    }
    Details->SourceDependenciesCount = cJSON_GetArraySize(SourceDependenciesElement);

    // Details->SourceDependencies
    Details->SourceDependencies = (char **)malloc(Details->SourceDependenciesCount * sizeof(char *));
    for (u32 SourceIndex = 0; SourceIndex < Details->SourceDependenciesCount; SourceIndex++)
    {
        Details->SourceDependencies[SourceIndex] = (char *)malloc(TARGET_FULL_PATH_BUFFER_SIZE);
        ZeroMemory(Details->SourceDependencies[SourceIndex], TARGET_FULL_PATH_BUFFER_SIZE);
        cJSON *HeaderDependency = cJSON_GetArrayItem(SourceDependenciesElement, SourceIndex);
        StringCchCatA(Details->SourceDependencies[SourceIndex], TARGET_FULL_PATH_BUFFER_SIZE, HeaderDependency->valuestring);
    }

    // Details->CompilerFlagsCount
    cJSON *CompilerFlagsElement = cJSON_GetObjectItem(JsonObject, "compiler_flags");
    if (!CompilerFlagsElement || !cJSON_IsArray(CompilerFlagsElement))
    {
        return FALSE;
    }
    Details->CompilerFlagsCount = cJSON_GetArraySize(CompilerFlagsElement);

    // Details->CompilerFlags
    Details->CompilerFlags = (char **)malloc(Details->CompilerFlagsCount * sizeof(char *));
    for (u32 FlagIndex = 0; FlagIndex < Details->CompilerFlagsCount; FlagIndex++)
    {
        Details->CompilerFlags[FlagIndex] = (char *)malloc(TARGET_FULL_PATH_BUFFER_SIZE);
        ZeroMemory(Details->CompilerFlags[FlagIndex], TARGET_FULL_PATH_BUFFER_SIZE);
        cJSON *CompilerFlag = cJSON_GetArrayItem(CompilerFlagsElement, FlagIndex);
        StringCchCatA(Details->CompilerFlags[FlagIndex], TARGET_FULL_PATH_BUFFER_SIZE, CompilerFlag->valuestring);
    }

    return TRUE;
}

b32 LoadWin32ExecutableFileTarget(target *Target, cJSON *JsonObject)
{
    win32_executable_file *Details = (win32_executable_file *)malloc(sizeof(win32_executable_file));
    Target->Details = Details;
    ZeroMemory(Details->ArtifactPath, ArrayCount(Details->ArtifactPath));
    cJSON *ArtifactPathItem = cJSON_GetObjectItem(JsonObject, "artifact_path");
    if (!ArtifactPathItem)
    {
        return FALSE;
    }
    StringCchCatA(Details->ArtifactPath, ArrayCount(Details->ArtifactPath), ArtifactPathItem->valuestring);

    // Details->ObjectDependenciesCount
    cJSON *ObjectDependenciesElement = cJSON_GetObjectItem(JsonObject, "object_dependencies");
    if (!ObjectDependenciesElement || !cJSON_IsArray(ObjectDependenciesElement))
    {
        return FALSE;
    }
    Details->ObjectDependenciesCount = cJSON_GetArraySize(ObjectDependenciesElement);

    // Details->ObjectDependencies
    Details->ObjectDependencies = (char **)malloc(Details->ObjectDependenciesCount * sizeof(char *));
    for (u32 ObjectIndex = 0; ObjectIndex < Details->ObjectDependenciesCount; ObjectIndex++)
    {
        Details->ObjectDependencies[ObjectIndex] = (char *)malloc(TARGET_FULL_PATH_BUFFER_SIZE);
        ZeroMemory(Details->ObjectDependencies[ObjectIndex], TARGET_FULL_PATH_BUFFER_SIZE);
        cJSON *ObjectDependency = cJSON_GetArrayItem(ObjectDependenciesElement, ObjectIndex);
        StringCchCatA(Details->ObjectDependencies[ObjectIndex], TARGET_FULL_PATH_BUFFER_SIZE, ObjectDependency->valuestring);
    }

    // Details->LinkerFlagsCount
    cJSON *LinkerFlagsElement = cJSON_GetObjectItem(JsonObject, "linker_flags");
    if (!LinkerFlagsElement || !cJSON_IsArray(LinkerFlagsElement))
    {
        return FALSE;
    }
    Details->LinkerFlagsCount = cJSON_GetArraySize(LinkerFlagsElement);

    // Details->LinkerFlags
    Details->LinkerFlags = (char **)malloc(Details->LinkerFlagsCount * sizeof(char *));
    for (u32 FlagIndex = 0; FlagIndex < Details->LinkerFlagsCount; FlagIndex++)
    {
        Details->LinkerFlags[FlagIndex] = (char *)malloc(TARGET_FULL_PATH_BUFFER_SIZE);
        ZeroMemory(Details->LinkerFlags[FlagIndex], TARGET_FULL_PATH_BUFFER_SIZE);
        cJSON *LinkerFlag = cJSON_GetArrayItem(LinkerFlagsElement, FlagIndex);
        StringCchCatA(Details->LinkerFlags[FlagIndex], TARGET_FULL_PATH_BUFFER_SIZE, LinkerFlag->valuestring);
    }

    return TRUE;
}

void LoadFileIntoTargetGraph(char *FilePath, char *BuildConfigurationDirectoryPath)
{
    char PackagePathOfFile[PACKAGE_PATH_BUFFER_SIZE] = {};
    file_path_segment_node *FilePathSegmentList = CreateFilePathSegmentList(FilePath);
    file_path_segment_node *BuildConfigurationDirectorySegmentList = CreateFilePathSegmentList(BuildConfigurationDirectoryPath);
    file_path_segment_node *RealPackagePathList = SkipSubPath(FilePathSegmentList, BuildConfigurationDirectorySegmentList);
    FreeFilePathSegmentList(BuildConfigurationDirectorySegmentList);
    FlattenPathSegmentList(RealPackagePathList, PackagePathOfFile, PACKAGE_PATH_BUFFER_SIZE, '/');
    FreeFilePathSegmentList(RealPackagePathList);
    RemoveLastSegmentFromPath(PackagePathOfFile, FALSE, '/');

    read_file_result File = ReadFileIntoMemory(FilePath);
    cJSON *FileRoot = cJSON_ParseWithLength((char *)File.FileMemory, File.Size);
    if (!FileRoot || !cJSON_IsArray(FileRoot))
    {
        ReportParseFailure(FilePath);
        return;
    }

    u32 ArraySize = cJSON_GetArraySize(FileRoot);
    for (u32 JsonObjectIndex = 0; JsonObjectIndex < ArraySize; JsonObjectIndex++)
    {
        cJSON *CurrentJsonObject = cJSON_GetArrayItem(FileRoot, JsonObjectIndex);
        if (!cJSON_IsObject(CurrentJsonObject))
        {
            ReportParseFailure(FilePath);
            return;
        }
        
        cJSON *TypeElement = cJSON_GetObjectItem(CurrentJsonObject, "type");
        if (!TypeElement)
        {
            ReportParseFailure(FilePath);
            return;
        }

        target_type TargetType = GetTargetTypeFromString(TypeElement->valuestring);
        cJSON *NameElement = cJSON_GetObjectItem(CurrentJsonObject, "name");
        target *Target = AddTarget(NameElement->valuestring, PackagePathOfFile, TargetType);

        b32 Succeeded = FALSE;
        switch (TargetType)
        {
            case TT_CPP_HEADER_FILE:
            {
                Succeeded = LoadCppHeaderTarget(Target, CurrentJsonObject);
            } break;

            case TT_CPP_SOURCE_FILE:
            {
                Succeeded = LoadCppSourceTarget(Target, CurrentJsonObject);
            } break;

            case TT_MSVC_OBJECT_FILE:
            {
                Succeeded = LoadMsvcObjectFileTarget(Target, CurrentJsonObject);
            } break;

            case TT_WIN32_EXECUTABLE_FILE:
            {
                Succeeded = LoadWin32ExecutableFileTarget(Target, CurrentJsonObject);
            } break;

            case TT_STRING_LIST:
            {
                // Succeeded = LoadStringListTarget(Target, CurrentJsonObject);
            } break;

            case TT_STRING:
            {
                // Succeeded = LoadXXXTarget(Target, CurrentJsonObject);
            } break;

            case TT_CONDITIONAL_STRING:
            {
                // Succeeded = LoadXXXTarget(Target, CurrentJsonObject);
            } break;

            case TT_BUILD_TIME_CONDITION:
            {
                // Succeeded = LoadXXXTarget(Target, CurrentJsonObject);
            } break;

            default:
            {
            } break;
        }
    }

    FreeFileMemory(File);
}