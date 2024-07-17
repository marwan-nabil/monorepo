#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>

#include "sources\win32\libraries\base_types.h"
#include "sources\win32\libraries\basic_defines.h"
#include "sources\win32\libraries\strings\path_handling.h"
#include "sources\win32\libraries\strings\strings.h"
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

static b32 LoadCppHeaderTarget(target *Target, cJSON *JsonObject)
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

static b32 LoadCppSourceTarget(target *Target, cJSON *JsonObject)
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

static b32 LoadMsvcObjectFileTarget(target *Target, cJSON *JsonObject)
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

static b32 LoadWin32ExecutableFileTarget(target *Target, cJSON *JsonObject)
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

static b32 LoadStringListTarget(target *Target, cJSON *JsonObject)
{
    string_list *Details = (string_list *)malloc(sizeof(string_list));
    Target->Details = Details;

    // Details->StringsCount
    cJSON *StringsElement = cJSON_GetObjectItem(JsonObject, "strings");
    if (!StringsElement || !cJSON_IsArray(StringsElement))
    {
        return FALSE;
    }
    Details->StringsCount = cJSON_GetArraySize(StringsElement);

    // Details->Strings
    for (u32 StringIndex = 0; StringIndex < Details->StringsCount; StringIndex++)
    {
        cJSON *String = cJSON_GetArrayItem(StringsElement, StringIndex);
        u32 AllocationSize = StringLength(String->valuestring);
        Details->Strings[StringIndex] = (char *)malloc(AllocationSize);
        StringCchCatA(Details->Strings[StringIndex], AllocationSize, String->valuestring);
    }

    return TRUE;
}

static b32 LoadStringTarget(target *Target, cJSON *JsonObject)
{
    string *Details = (string *)malloc(sizeof(string));
    Target->Details = Details;

    // Details->Value
    cJSON *ValueElement = cJSON_GetObjectItem(JsonObject, "value");
    u32 AllocationSize = StringLength(ValueElement->valuestring);
    Details->Value = (char *)malloc(AllocationSize);
    StringCchCatA(Details->Value, AllocationSize, ValueElement->valuestring);
    return TRUE;
}

static b32 LoadConditionalStringTarget(target *Target, cJSON *JsonObject)
{
    conditional_string *Details = (conditional_string *)malloc(sizeof(conditional_string));
    Target->Details = Details;

    // Details->ConditionalStringValuesCount
    cJSON *ConditionalStringValuesElement = cJSON_GetObjectItem(JsonObject, "conditional_string_values");
    if (!ConditionalStringValuesElement || !cJSON_IsArray(ConditionalStringValuesElement))
    {
        return FALSE;
    }
    Details->ConditionalStringValuesCount = cJSON_GetArraySize(ConditionalStringValuesElement);
    
    // Details->ConditionalStringValues
    for
    (
        u32 ConditionalStringValueIndex = 0;
        ConditionalStringValueIndex < Details->ConditionalStringValuesCount;
        ConditionalStringValueIndex++
    )
    {
        Details->ConditionalStringValues[ConditionalStringValueIndex] =
            (conditional_string_value *)malloc(sizeof(conditional_string_value));
        cJSON *CSVObject = cJSON_GetArrayItem(ConditionalStringValuesElement, ConditionalStringValueIndex);

        cJSON *ConditionObject = cJSON_GetObjectItem(CSVObject, "condition");
        u32 AllocationSize = StringLength(ConditionObject->valuestring);
        Details->ConditionalStringValues[ConditionalStringValueIndex]->Condition =
            (char *)malloc(AllocationSize);
        StringCchCatA
        (
            Details->ConditionalStringValues[ConditionalStringValueIndex]->Condition,
            AllocationSize,
            ConditionObject->valuestring
        );
        
        cJSON *ValueObject = cJSON_GetObjectItem(CSVObject, "value");
        AllocationSize = StringLength(ValueObject->valuestring);
        Details->ConditionalStringValues[ConditionalStringValueIndex]->Value =
            (char *)malloc(AllocationSize);
        StringCchCatA
        (
            Details->ConditionalStringValues[ConditionalStringValueIndex]->Value,
            AllocationSize,
            ValueObject->valuestring
        );
    }

    return TRUE;
}

static b32 LoadBuildTimeConditionTarget(target *Target, cJSON *JsonObject)
{
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
                Succeeded = LoadStringListTarget(Target, CurrentJsonObject);
            } break;

            case TT_STRING:
            {
                Succeeded = LoadStringTarget(Target, CurrentJsonObject);
            } break;

            case TT_CONDITIONAL_STRING:
            {
                Succeeded = LoadConditionalStringTarget(Target, CurrentJsonObject);
            } break;

            case TT_BUILD_TIME_CONDITION:
            {
                Succeeded = LoadBuildTimeConditionTarget(Target, CurrentJsonObject);
            } break;

            default:
            {
            } break;
        }
    }

    FreeFileMemory(File);
}