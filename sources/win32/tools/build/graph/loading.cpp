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
#include "loading.h"

static void ReportParseFailure(char *FilePath)
{
    ConsoleSwitchColor(FOREGROUND_RED);
    printf("ERROR: failed to parse input build description file.\n");
    printf("ERROR:      Error location: %s\n", FilePath);
    ConsoleResetColor();
}

b32 LoadCppHeaderBuildObject(build_object *BuildObject, cJSON *JsonObject)
{
    cpp_header_file *Contents = (cpp_header_file *)malloc(sizeof(cpp_header_file));
    BuildObject->Contents = Contents;
    ZeroMemory(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath));
    cJSON *ArtifactPathItem = cJSON_GetObjectItem(JsonObject, "artifact_path");
    if (!ArtifactPathItem)
    {
        return FALSE;
    }
    StringCchCatA(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath), ArtifactPathItem->valuestring);
    return TRUE;
}

b32 LoadCppSourceBuildObject(build_object *BuildObject, cJSON *JsonObject)
{
    cpp_source_file *Contents = (cpp_source_file *)malloc(sizeof(cpp_source_file));
    BuildObject->Contents = Contents;
    ZeroMemory(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath));
    cJSON *ArtifactPathItem = cJSON_GetObjectItem(JsonObject, "artifact_path");
    if (!ArtifactPathItem)
    {
        return FALSE;
    }
    StringCchCatA(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath), ArtifactPathItem->valuestring);

    // Contents->HeaderDependenciesCount
    cJSON *HeaderDependenciesElement = cJSON_GetObjectItem(JsonObject, "header_dependencies");
    if (!HeaderDependenciesElement || !cJSON_IsArray(HeaderDependenciesElement))
    {
        return FALSE;
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
    return TRUE;
}

b32 LoadMsvcObjectFileBuildObject(build_object *BuildObject, cJSON *JsonObject)
{
    msvc_object_file *Contents = (msvc_object_file *)malloc(sizeof(msvc_object_file));
    BuildObject->Contents = Contents;
    ZeroMemory(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath));
    cJSON *ArtifactPathItem = cJSON_GetObjectItem(JsonObject, "artifact_path");
    if (!ArtifactPathItem)
    {
        return FALSE;
    }
    StringCchCatA(Contents->ArtifactPath, ArrayCount(Contents->ArtifactPath), ArtifactPathItem->valuestring);

    // Contents->SourceDependenciesCount
    cJSON *SourceDependenciesElement = cJSON_GetObjectItem(JsonObject, "source_dependencies");
    if (!SourceDependenciesElement || !cJSON_IsArray(SourceDependenciesElement))
    {
        return FALSE;
    }
    Contents->SourceDependenciesCount = cJSON_GetArraySize(SourceDependenciesElement);

    // Contents->SourceDependencies
    Contents->SourceDependencies = (char **)malloc(Contents->SourceDependenciesCount * sizeof(char *));
    for (u32 SourceIndex = 0; SourceIndex < Contents->SourceDependenciesCount; SourceIndex++)
    {
        Contents->SourceDependencies[SourceIndex] = (char *)malloc(BUILD_OBJECT_NAME_REFERENCE_LENGTH);
        ZeroMemory(Contents->SourceDependencies[SourceIndex], BUILD_OBJECT_NAME_REFERENCE_LENGTH);
        cJSON *HeaderDependency = cJSON_GetArrayItem(SourceDependenciesElement, SourceIndex);
        StringCchCatA(Contents->SourceDependencies[SourceIndex], BUILD_OBJECT_NAME_REFERENCE_LENGTH, HeaderDependency->valuestring);
    }

    // Contents->CompilerFlagsCount
    cJSON *CompilerFlagsElement = cJSON_GetObjectItem(JsonObject, "compiler_flags");
    if (!CompilerFlagsElement || !cJSON_IsArray(CompilerFlagsElement))
    {
        return FALSE;
    }
    Contents->CompilerFlagsCount = cJSON_GetArraySize(CompilerFlagsElement);

    // Contents->CompilerFlags
    Contents->CompilerFlags = (char **)malloc(Contents->CompilerFlagsCount * sizeof(char *));
    for (u32 FlagIndex = 0; FlagIndex < Contents->CompilerFlagsCount; FlagIndex++)
    {
        Contents->CompilerFlags[FlagIndex] = (char *)malloc(BUILD_OBJECT_NAME_REFERENCE_LENGTH);
        ZeroMemory(Contents->CompilerFlags[FlagIndex], BUILD_OBJECT_NAME_REFERENCE_LENGTH);
        cJSON *CompilerFlag = cJSON_GetArrayItem(CompilerFlagsElement, FlagIndex);
        StringCchCatA(Contents->CompilerFlags[FlagIndex], BUILD_OBJECT_NAME_REFERENCE_LENGTH, CompilerFlag->valuestring);
    }

    return TRUE;
}

void ParseFileIntoBuildObjects(char *FilePath, char *BuildConfigurationDirectoryPath)
{
    char PackagePathOfFile[BUILD_PACKAGE_PATH_LENGTH] = {};
    file_path_segment_node *FilePathSegmentList = CreateFilePathSegmentList(FilePath);
    file_path_segment_node *BuildConfigurationDirectorySegmentList = CreateFilePathSegmentList(BuildConfigurationDirectoryPath);
    file_path_segment_node *RealPackagePathList = SkipSubPath(FilePathSegmentList, BuildConfigurationDirectorySegmentList);
    FreeFilePathSegmentList(BuildConfigurationDirectorySegmentList);
    FlattenPathSegmentList(RealPackagePathList, PackagePathOfFile, BUILD_PACKAGE_PATH_LENGTH, '/');
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

        build_object_type ObjectType = GetBuildObjectTypeFromString(TypeElement->valuestring);
        cJSON *NameElement = cJSON_GetObjectItem(CurrentJsonObject, "name");
        build_object *BuildObject = AddBuildObject(NameElement->valuestring, ObjectType, PackagePathOfFile);

        b32 Succeeded = FALSE;
        switch (ObjectType)
        {
            case BOT_CPP_HEADER_FILE:
            {
                Succeeded = LoadCppHeaderBuildObject(BuildObject, CurrentJsonObject);
            } break;

            case BOT_CPP_SOURCE_FILE:
            {
                Succeeded = LoadCppSourceBuildObject(BuildObject, CurrentJsonObject);
            } break;

            case BOT_MSVC_OBJECT_FILE:
            {
                Succeeded = LoadMsvcObjectFileBuildObject(BuildObject, CurrentJsonObject);
            } break;

            case BOT_WIN32_EXECUTABLE_FILE:
            {
                // Succeeded = LoadWin32ExecutableFileBuildObject(BuildObject, CurrentJsonObject);
            } break;

            case BOT_STRING_LIST:
            case BOT_STRING:
            case BOT_CONDITIONAL_STRING:
            case BOT_BUILD_TIME_CONDITION:
            {
            } break;
        }
    }

    FreeFileMemory(File);
}