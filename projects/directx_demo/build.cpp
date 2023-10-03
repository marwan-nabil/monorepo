b32 BuildDirectxDemo(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\directx_demo\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /FC /Oi /GR- /EHa- /Zi /MD");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    if (BuildContext->argc < 3)
    {
        ConsolePrintColored("ERROR: invalid number of arguments for build directx_demo.\n", FOREGROUND_RED);
        DisplayHelp();
        return FALSE;
    }
    else if(strcmp(BuildContext->argv[2], "debug") == 0)
    {
        AddCompilerFlags(BuildContext, "/D_DEBUG=1");
        SetOuputBinaryPath(BuildContext, "\\directx_demo_debug.exe");
    }
    else if(strcmp(BuildContext->argv[2], "release") == 0)
    {
        SetOuputBinaryPath(BuildContext, "\\directx_demo.exe");
    }
    else
    {
        ConsoleSwitchColor(FOREGROUND_RED);
        printf("ERROR: invalid argument \"%s\" for build directx_demo ...\n", BuildContext->argv[2]);
        ConsoleResetColor();
        DisplayHelp();
        return FALSE;
    }

    AddLinkerFlags(BuildContext, "/subsystem:windows /incremental:no /opt:ref");
    AddLinkerFlags(BuildContext, "user32.lib winmm.lib d3d11.lib dxgi.lib d3dcompiler.lib");

    b32 BuildSuccess = CompileCpp(BuildContext);

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\directx_demo\\vertex_shader.hlsl");
    AddCompilerFlags(BuildContext, "/T vs_4_0 /Od /Zi /E SimpleVertexShader /Fh vertex_shader.h /Vn GlobalVertexShader");
    SetOuputBinaryPath(BuildContext, "\\vertex_shader.cso");

    BuildSuccess = BuildSuccess && CompileShader(BuildContext);

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\directx_demo\\pixel_shader.hlsl");
    AddCompilerFlags(BuildContext, "/T ps_4_0 /Od /Zi /E SimplePixelShader /Fh pixel_shader.h /Vn GlobalPixelShader");
    SetOuputBinaryPath(BuildContext, "\\pixel_shader.cso");

    BuildSuccess = BuildSuccess && CompileShader(BuildContext);
    return BuildSuccess;
}