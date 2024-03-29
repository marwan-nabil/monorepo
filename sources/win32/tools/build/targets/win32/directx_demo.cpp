#include <Windows.h>
#include <stdint.h>
#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\strings\string_list.h"

#include "..\..\build.h"
#include "..\..\helpers\build_helpers.h"
#include "..\..\helpers\compiler_helpers.h"

b32 BuildDirectxDemo(build_context *BuildContext)
{
    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\directx\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /FC /Oi /O2 /GR- /EHa- /Zi /MD");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    SetCompilerIncludePath(BuildContext, "\\");

    AddLinkerFlags(BuildContext, "/subsystem:windows /incremental:no /opt:ref");
    AddLinkerFlags(BuildContext, "user32.lib winmm.lib d3d11.lib dxgi.lib D3DCompiler.lib");

    SetLinkerOutputBinary(BuildContext, "\\directx_demo.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\directx\\vertex_shader.hlsl");
    AddCompilerFlags
    (
        BuildContext,
        "/T vs_4_0 /Od /Zi /E SimpleVertexShader /Vn GlobalVertexShader"
    );
    SetCompilerOutputObject(BuildContext, "\\vertex_shader.cso");
    BuildSuccess = CompileShader(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\directx\\pixel_shader.hlsl");
    AddCompilerFlags
    (
        BuildContext,
        "/T ps_4_0 /Od /Zi /E SimplePixelShader /Vn GlobalPixelShader"
    );
    SetCompilerOutputObject(BuildContext, "\\pixel_shader.cso");
    BuildSuccess = CompileShader(BuildContext);
    return BuildSuccess;
}