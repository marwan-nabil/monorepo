#pragma once

#define BUILD_OBJECT_NAME_LENGTH 64
#define ARTIFACT_MAX_FILE_PATH 1024

enum build_object_type
{
    BOT_STRING,
    BOT_STRING_LIST,
    BOT_REFERENCE_LIST,
    BOT_BUILD_TIME_STRING,
    BOT_CPP_HEADER_FILE,
    BOT_CPP_SOURCE_FILE,
    BOT_WIN32_EXECUTABLE_FILE,
    BOT_MSVC_OBJECT_FILE,
    BOT_BUILD_TIME_CONDITION,
};

struct build_object
{
    char Name[BUILD_OBJECT_NAME_LENGTH];
    build_object_type Type;
    void *Contents;
};

struct build_time_condition
{
    u32 CommandLineArgumentIndex;
    char *CommandLineArgumentValue;
};

struct string
{
    char *Value;
};

struct string_list
{
    u32 Count;
    char **Strings;
};

struct string_reference_list
{
    u32 Count;
    char **StringReferences;
};

struct conditional_string_value
{
    char *ConditionName;
    char *Value;
};

struct conditional_string
{
    u32 Count;
    conditional_string_value **ConditionalStringValues;
};

struct cpp_header_file
{
    char ArtifactPath[ARTIFACT_MAX_FILE_PATH];
};

struct cpp_source_file
{
    char ArtifactPath[ARTIFACT_MAX_FILE_PATH];
    char **HeaderDependencies;
    u32 HeaderDependenciesCount;
};

struct msvc_object_file
{
    char ArtifactPath[ARTIFACT_MAX_FILE_PATH];
    char *CompilerFlags;
    char **SourceDependencies;
    u32 SourceDependenciesCount;
};

struct win32_executable_file
{
    char ArtifactPath[ARTIFACT_MAX_FILE_PATH];
    char *LinkerFlags;
    char **ObjectDependencies;
    u32 ObjectDependenciesCount;
};