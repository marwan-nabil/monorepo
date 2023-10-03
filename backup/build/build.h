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

extern target_mapping BuildTargetMappings[1];
void AddCompilerFlags(build_context *BuildContext, const char *Flags);
void AddLinkerFlags(build_context *BuildContext, const char *Flags);
void AddSourceFile(build_context *BuildContext, const char *SourceFile);
void SetOuputBinaryPath(build_context *BuildContext, const char *OutputBinaryName);
void ClearBuildContext(build_context *BuildContext);
b32 CompileShader(build_context *BuildContext);
b32 CompileCpp(build_context *BuildContext);
b32 CompileAssembly(build_context *BuildContext);
b32 BuildBuild(build_context *BuildContext);
b32 BuildLint(build_context *BuildContext);