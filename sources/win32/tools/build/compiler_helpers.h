#pragma once

b32 AssembleWithNasm(build_context *BuildContext);
b32 CompileWithGCC(build_context *BuildContext);
b32 LinkWithGCC(build_context *BuildContext);
b32 CompileShader(build_context *BuildContext);
b32 CompileWithMSVC(build_context *BuildContext);