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

struct build_object_table_entry
{
    build_object BuildObject;
    build_object_table_entry *NextEntry;
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
    char **Strings;
    u32 StringsCount;
};

struct string_reference_list
{
    char **StringReferences;
    u32 StringReferencesCount;
};

struct conditional_string_value
{
    char *Condition;
    char *Value;
};

struct conditional_string
{
    conditional_string_value **ConditionalStringValues;
    u32 ConditionalStringValuesCount;
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

build_object *GetBuildObject(char *Name);
build_object *AddBuildObject(char *Name, build_object_type Type);