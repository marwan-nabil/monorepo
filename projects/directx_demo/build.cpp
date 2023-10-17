b32 BuildDirectxDemo(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\directx_demo\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /I.. /FC /Oi /O2 /GR- /EHa- /Zi /MD");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:windows /incremental:no /opt:ref");
    AddLinkerFlags(BuildContext, "user32.lib winmm.lib d3d11.lib dxgi.lib D3DCompiler.lib");

    SetOuputBinaryPath(BuildContext, "\\directx_demo.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\directx_demo\\vertex_shader.hlsl");
    AddCompilerFlags
    (
        BuildContext,
        "/T vs_4_0 /Od /Zi /E SimpleVertexShader /Vn GlobalVertexShader"
    );
    SetOuputBinaryPath(BuildContext, "\\vertex_shader.cso");

    BuildSuccess = CompileShader(BuildContext);
    if (!BuildSuccess)
    {
        return FALSE;
    }

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\directx_demo\\pixel_shader.hlsl");
    AddCompilerFlags
    (
        BuildContext,
        "/T ps_4_0 /Od /Zi /E SimplePixelShader /Vn GlobalPixelShader"
    );
    SetOuputBinaryPath(BuildContext, "\\pixel_shader.cso");

    BuildSuccess = CompileShader(BuildContext);
    return BuildSuccess;
}