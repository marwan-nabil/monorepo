#pragma once

void AddCompilerFlags(build_context *BuildContext, const char *Flags);
void SetCompilerIncludePath(build_context *BuildContext, const char *IncludePath);
void AddCompilerSourceFile(build_context *BuildContext, const char *SourceFile);
void SetCompilerOutputObject(build_context *BuildContext, const char *ObjectFile);
void AddLinkerFlags(build_context *BuildContext, const char *Flags);
void SetLinkerScriptPath(build_context *BuildContext, const char *LinkerScriptFile);
void AddLinkerInputFile(build_context *BuildContext, const char *LinkerInputFile);
void SetLinkerOutputBinary(build_context *BuildContext, const char *OutputBinaryPath);
void PushSubTarget(build_context *BuildContext, const char *SubTargetRelativePath);
void PopSubTarget(build_context *BuildContext);
void FreeFileNameList(file_name_node *RootNode);
void FlattenFileNameList(file_name_node *FileNameList, char *Output, u32 OutputSize);
void ClearBuildContext(build_context *BuildContext);