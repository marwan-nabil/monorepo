#pragma once

struct build_context
{
    i32 argc;
    char **argv;

    console_context *ConsoleContext;

    char OutputDirectoryPath[1024];
    char RootDirectoryPath[1024];

    char CompilerFlags[1024];
    char LinkerFlags[1024];
    char SourcesString[1024];
    char OutputBinaryPath[1024];
};

typedef b32 (build_function_type)(build_context *);

struct target_mapping
{
    const char *TargetName;
    build_function_type *BuildFunction;
};