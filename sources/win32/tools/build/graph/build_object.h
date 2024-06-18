#pragma once

#define BUILD_OBJECT_NAME_LENGTH 64
#define BUILD_PACKAGE_PATH_LENGTH 256
#define BUILD_OBJECT_NAME_REFERENCE_LENGTH 1024
#define ARTIFACT_MAX_FILE_PATH 1024

enum build_object_type
{
    BOT_INVALID,
    BOT_STRING,
    BOT_STRING_LIST,
    BOT_CONDITIONAL_STRING,
    BOT_BUILD_TIME_CONDITION,
    BOT_CPP_HEADER_FILE,
    BOT_CPP_SOURCE_FILE,
    BOT_WIN32_EXECUTABLE_FILE,
    BOT_MSVC_OBJECT_FILE,
};

struct build_object
{
    char Name[BUILD_OBJECT_NAME_LENGTH];
    char PackagePath[BUILD_PACKAGE_PATH_LENGTH];
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
    char **SourceDependencies;
    char **CompilerFlags;
    u32 SourceDependenciesCount;
    u32 CompilerFlagsCount;
};

struct win32_executable_file
{
    char ArtifactPath[ARTIFACT_MAX_FILE_PATH];
    char **LinkerFlags;
    char **ObjectDependencies;
    u32 ObjectDependenciesCount;
    u32 LinkerFlagsCount;
};

build_object *GetBuildObject(char *Name);
build_object *AddBuildObject(char *Name, build_object_type Type, char *PackagePath);
build_object_type GetBuildObjectTypeFromString(char *TypeString);