#pragma once

struct environment_info
{
    i32 argc;
    char **argv;
    char RootDirectoryPath[1024];
    char OutputDirectoryPath[1024];
    char TargetOutputDirectoryPath[1024];
};

struct compilation_info
{
    char CompilerFlags[1024];
    char CompilerIncludePath[1024];
    string_node *Sources;
    char OutputObjectPath[1024];
};

struct linking_info
{
    char LinkerFlags[1024];
    char LinkerScriptPath[1024];
    string_node *LinkerInputs;
    char OutputBinaryPath[1024];
};

struct build_context
{
    environment_info EnvironmentInfo;
    compilation_info CompilationInfo;
    linking_info LinkingInfo;
};

void DisplayHelp();