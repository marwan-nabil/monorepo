#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\strings\string_list.h"
#include "sources\win32\strings\strings.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

b32 BuildRefTerm(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\refterm\\refterm.hlsl");
    AddCompilerFlags(BuildContext, "/nologo /T cs_5_0 /E ComputeMain /O3 /WX /Fh refterm_cs.h /Vn ReftermCSShaderBytes /Qstrip_reflect /Qstrip_debug /Qstrip_priv");
    b32 BuildSuccess = CompileShader2(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\refterm\\refterm.hlsl");
    AddCompilerFlags(BuildContext, "/nologo /T ps_5_0 /E PixelMain /O3 /WX /Fh refterm_ps.h /Vn ReftermPSShaderBytes /Qstrip_reflect /Qstrip_debug /Qstrip_priv");
    BuildSuccess = CompileShader2(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\refterm\\refterm.hlsl");
    AddCompilerFlags(BuildContext, "/nologo /T vs_5_0 /E VertexMain /O3 /WX /Fh refterm_vs.h /Vn ReftermVSShaderBytes /Qstrip_reflect /Qstrip_debug /Qstrip_priv");
    BuildSuccess = CompileShader2(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    char SourceFileName[1024] = {};
    StringCchCat(SourceFileName, 1024, BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    StringCchCat(SourceFileName, 1024, "\\refterm_cs.h");

    char DestinationFileName[1024] = {};
    StringCchCat(DestinationFileName, 1024, BuildContext->EnvironmentInfo.RootDirectoryPath);
    StringCchCat(DestinationFileName, 1024, "\\sources\\win32\\demos\\refterm\\refterm_cs.h");

    MoveFile(SourceFileName, DestinationFileName);

    return BuildSuccess;
}