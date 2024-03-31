#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\strings\string_list.h"
#include "sources\win32\strings\strings.h"
#include "sources\win32\strings\path_handling.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

static b32 BuildShaders(build_context *BuildContext)
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

    char *FilesToMove[3] = 
    {
        "refterm_cs.h",
        "refterm_vs.h",
        "refterm_ps.h",
    };

    char SourceFileName[1024] = {};
    StringCchCat(SourceFileName, 1024, BuildContext->EnvironmentInfo.TargetOutputDirectoryPath);
    StringCchCat(SourceFileName, 1024, "\\");

    char DestinationFileName[1024] = {};
    StringCchCat(DestinationFileName, 1024, BuildContext->EnvironmentInfo.RootDirectoryPath);
    StringCchCat(DestinationFileName, 1024, "\\sources\\win32\\demos\\refterm\\");

    for (u8 FileIndex = 0; FileIndex < ArrayCount(FilesToMove); FileIndex++)
    {
        // StringCchCat(SourceFileName, 1024, "\\");
        StringCchCat(SourceFileName, 1024, FilesToMove[FileIndex]);
        // StringCchCat(DestinationFileName, 1024, "\\");
        StringCchCat(DestinationFileName, 1024, FilesToMove[FileIndex]);
        MoveFile(SourceFileName, DestinationFileName);
        RemoveLastSegmentFromPath(SourceFileName, TRUE);
        RemoveLastSegmentFromPath(DestinationFileName, TRUE);
    }

    return BuildSuccess;
}

b32 BuildRefTerm(build_context *BuildContext)
{
    b32 BuildSuccess = BuildShaders(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    char *SharedCompilerFlags = "/nologo /W3 /Z7 /GS- /Gs999999";
    char *SharedLinkerFlags = "/incremental:no /opt:icf /opt:ref";
    char *SharedSourceFiles[] =
    {
        "\\sources\\win32\\demos\\refterm\\refterm.c",
        "\\sources\\win32\\demos\\refterm\\refterm_example_dwrite.cpp"
    };

    AddCompilerSourceFile(BuildContext, SharedSourceFiles[0]);
    AddCompilerSourceFile(BuildContext, SharedSourceFiles[1]);
    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/D_DEBUG /Od");
    SetCompilerIncludePath(BuildContext, "\\");
    AddLinkerFlags(BuildContext, SharedLinkerFlags);
    AddLinkerFlags(BuildContext, "/subsystem:windows");
    SetLinkerOutputBinary(BuildContext, "\\refterm_debug_msvc.exe");
    BuildSuccess = CompileWithMSVC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, SharedSourceFiles[0]);
    AddCompilerSourceFile(BuildContext, SharedSourceFiles[1]);
    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/O2");
    SetCompilerIncludePath(BuildContext, "\\");
    AddLinkerFlags(BuildContext, SharedLinkerFlags);
    AddLinkerFlags(BuildContext, "/subsystem:windows");
    SetLinkerOutputBinary(BuildContext, "\\refterm_release_msvc.exe");
    BuildSuccess = CompileWithMSVC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\refterm\\splat.cpp");
    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/O2");
    SetCompilerIncludePath(BuildContext, "\\");
    AddLinkerFlags(BuildContext, SharedLinkerFlags);
    AddLinkerFlags(BuildContext, "/subsystem:console");
    SetLinkerOutputBinary(BuildContext, "\\splat.exe");
    BuildSuccess = CompileWithMSVC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\refterm\\splat2.cpp");
    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/O2");
    SetCompilerIncludePath(BuildContext, "\\");
    AddLinkerFlags(BuildContext, SharedLinkerFlags);
    AddLinkerFlags(BuildContext, "/subsystem:console");
    SetLinkerOutputBinary(BuildContext, "\\splat2.exe");
    BuildSuccess = CompileWithMSVC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    return BuildSuccess;
}