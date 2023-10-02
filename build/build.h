#pragma once

struct build_context;
typedef b32 (build_function_type)(build_context *);

struct target_mapping
{
    const char *TargetName;
    build_function_type *BuildFunction;
    const char *BuildFilePath;
};

struct build_context
{
    i32 argc;
    char **argv;

    char OutputDirectoryPath[1024];
    char RootDirectoryPath[1024];

    char CompilerFlags[1024];
    char LinkerFlags[1024];
    char SourcesString[1024];
    char OutputBinaryPath[1024];
};