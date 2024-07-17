#pragma once

#define TARGET_NAME_BUFFER_SIZE 64
#define PACKAGE_PATH_BUFFER_SIZE 256
#define TARGET_FULL_PATH_BUFFER_SIZE 512
#define FILE_PATH_BUFFER_SIZE 1024

enum target_type
{
    TT_INVALID,
    TT_STRING,
    TT_STRING_LIST,
    TT_CONDITIONAL_STRING,
    TT_BUILD_TIME_CONDITION,
    TT_CPP_HEADER_FILE,
    TT_CPP_SOURCE_FILE,
    TT_WIN32_EXECUTABLE_FILE,
    TT_MSVC_OBJECT_FILE,
};

struct target
{
    char Name[TARGET_NAME_BUFFER_SIZE];
    char PackagePath[PACKAGE_PATH_BUFFER_SIZE];
    target_type Type;
    void *Details;
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
    char ArtifactPath[FILE_PATH_BUFFER_SIZE];
};

struct cpp_source_file
{
    char ArtifactPath[FILE_PATH_BUFFER_SIZE];
    char **HeaderDependencies;
    u32 HeaderDependenciesCount;
};

struct msvc_object_file
{
    char ArtifactPath[FILE_PATH_BUFFER_SIZE];
    char **SourceDependencies;
    char **CompilerFlags;
    u32 SourceDependenciesCount;
    u32 CompilerFlagsCount;
};

struct win32_executable_file
{
    char ArtifactPath[FILE_PATH_BUFFER_SIZE];
    char **LinkerFlags;
    char **ObjectDependencies;
    u32 ObjectDependenciesCount;
    u32 LinkerFlagsCount;
};

target *GetTarget(char *Name);
target *AddTarget(char *Name, char *PackagePath, target_type Type);
target_type GetTargetTypeFromString(char *TypeString);