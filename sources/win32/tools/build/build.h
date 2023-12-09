#pragma once

#define MAX_FILENAME 1024

struct file_name_node
{
    char FileName[MAX_FILENAME];
    file_name_node *NextFileName;
};

struct environment_info
{
    i32 argc;
    char **argv;
    console_context *ConsoleContext;
    char RootDirectoryPath[1024];
    char OutputDirectoryPath[1024];
    char TargetOutputDirectoryPath[1024];
};

struct compilation_info
{
    char CompilerFlags[1024];
    char CompilerIncludePath[1024];
    file_name_node *Sources;
    char OutputObjectPath[1024];
};

struct linking_info
{
    char LinkerFlags[1024];
    char LinkerScriptPath[1024];
    file_name_node *LinkerInputs;
    char OutputBinaryPath[1024];
};

struct build_context
{
    environment_info EnvironmentInfo;
    compilation_info CompilationInfo;
    linking_info LinkingInfo;
};

typedef b32 (build_function_type)(build_context *);

struct build_target_config
{
    const char *TargetName;
    build_function_type *BuildFunction;
    const char *FirstArgument;
    const char *SecondArgument;
    const char *ThirdArgument;
};

void DisplayHelp();