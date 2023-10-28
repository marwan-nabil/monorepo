#pragma once

struct build_context
{
    i32 argc;
    char **argv;

    console_context *ConsoleContext;

    char OutputDirectoryPath[1024];
    char TargetOutputDirectoryPath[1024];
    char RootDirectoryPath[1024];

    char CompilerFlags[1024];
    char LinkerFlags[1024];
    char SourcesString[1024];
    char LinkerInputsString[1024];
    char OutputBinaryPath[1024];
    char CompilerIncludePath[1024];
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

static void DisplayHelp();