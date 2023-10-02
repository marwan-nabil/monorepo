b32 BuildBuild(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\build\\build.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /Oi /FC /Od /GR- /EHa-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\build.temp.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}

b32 BuildBasicApp(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\basic_app\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:windows /incremental:no /opt:ref user32.lib gdi32.lib winmm.lib");

    SetOuputBinaryPath(BuildContext, "\\basic_app.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}

b32 BuildRayTracer(build_context *BuildContext)
{
    if (BuildContext->argc < 4)
    {
        ConsolePrintColored("ERROR: invalid number of arguments for build ray_tracer ...\n", FOREGROUND_RED);
        DisplayHelp();
        return FALSE;
    }

    AddSourceFile(BuildContext, "\\projects\\ray_tracer\\main.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib gdi32.lib");

    if (strcmp(BuildContext->argv[2], "optimized") == 0)
    {
        AddCompilerFlags(BuildContext, "/O2");
    }
    else if (strcmp(BuildContext->argv[2], "non_optimized") == 0)
    {
        AddCompilerFlags(BuildContext, "/Od");
    }
    else
    {
        ConsoleSwitchColor(FOREGROUND_RED);
        printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", BuildContext->argv[2]);
        ConsoleResetColor();
        DisplayHelp();
        return FALSE;
    }

    if (strcmp(BuildContext->argv[3], "1_lane") == 0)
    {
        AddCompilerFlags(BuildContext, "/DSIMD_NUMBEROF_LANES=1");
        SetOuputBinaryPath(BuildContext, "\\ray_tracer_1.exe");
    }
    else if (strcmp(BuildContext->argv[3], "4_lanes") == 0)
    {
        AddCompilerFlags(BuildContext, "/DSIMD_NUMBEROF_LANES=4");
        SetOuputBinaryPath(BuildContext, "\\ray_tracer_4.exe");
    }
    else if (strcmp(BuildContext->argv[3], "8_lanes") == 0)
    {
        AddCompilerFlags(BuildContext, "/DSIMD_NUMBEROF_LANES=8");
        SetOuputBinaryPath(BuildContext, "\\ray_tracer_8.exe");
    }
    else
    {
        ConsoleSwitchColor(FOREGROUND_RED);
        printf("ERROR: invalid argument \"%s\" for build ray_tracer ...\n", BuildContext->argv[3]);
        ConsoleResetColor();
        DisplayHelp();
        return FALSE;
    }

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}

b32 BuildImguiDemo(build_context *BuildContext)
{
    if (BuildContext->argc < 3)
    {
        ConsolePrintColored("ERROR: invalid number of arguments for build imgui_demo ...\n", FOREGROUND_RED);
        DisplayHelp();
        return FALSE;
    }

    AddSourceFile(BuildContext, "\\third_party\\imgui\\imgui*.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Zi /MD /utf-8");
    AddCompilerFlags(BuildContext, "/DUNICODE /D_UNICODE /DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");

    AddLinkerFlags(BuildContext, "user32.lib Gdi32.lib dwmapi.lib");

    if (strcmp(BuildContext->argv[2], "opengl2") == 0)
    {
        AddSourceFile(BuildContext, "\\projects\\imgui_demo\\main_opengl2.cpp");
        AddLinkerFlags(BuildContext, "opengl32.lib");
        SetOuputBinaryPath(BuildContext, "\\imgui_demo_opengl2.exe");
    }
    else if (strcmp(BuildContext->argv[2], "dx11") == 0)
    {
        AddSourceFile(BuildContext, "\\projects\\imgui_demo\\main_dx11.cpp");
        AddLinkerFlags(BuildContext, "d3d11.lib d3dcompiler.lib");
        SetOuputBinaryPath(BuildContext, "\\imgui_demo_dx11.exe");
    }
    else
    {
        ConsoleSwitchColor(FOREGROUND_RED);
        printf("ERROR: invalid argument \"%s\" for build imgui_demo ...\n", BuildContext->argv[2]);
        ConsoleResetColor();
        DisplayHelp();
        return FALSE;
    }

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}

b32 BuildHandmadeHero(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\handmade_hero\\game.cpp");

    char SharedCompilerFlags[1024];
    ZeroMemory(SharedCompilerFlags, ArrayCount(SharedCompilerFlags));
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/nologo /Zi /FC /Od /Oi /GR- /EHa- /Gm- /MTd ");
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 ");
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/DHANDMADE_WIN32=1 /DHANDMADE_SLOW=1 /DHANDMADE_INTERNAL=1 /DENABLE_ASSERTIONS ");

    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/LD /Fmgame.map");

    AddLinkerFlags(BuildContext, "/incremental:no");
    AddLinkerFlags(BuildContext, "/EXPORT:GameGetSoundSamples /EXPORT:GameUpdateAndRender");

    char PdbLinkerFlag[1024];
    ZeroMemory(PdbLinkerFlag, ArrayCount(PdbLinkerFlag));
    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), "/PDB:game_");

    char RandomString[5];
    ZeroMemory(RandomString, ArrayCount(RandomString));
    u32 RandomNumber;
    rand_s(&RandomNumber);
    RandomNumber = (u32)((f32)RandomNumber / (f32)UINT_MAX * 9999.0f);
    StringCchPrintfA(RandomString, ArrayCount(RandomString), "%d", RandomNumber);

    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), RandomString);
    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), ".pdb");

    AddLinkerFlags(BuildContext, PdbLinkerFlag);

    SetOuputBinaryPath(BuildContext, "\\game.dll");

    char LockFilePath[MAX_PATH];
    ZeroMemory(LockFilePath, ArrayCount(LockFilePath));
    StringCchCatA(LockFilePath, ArrayCount(LockFilePath), "compilation.lock");

    CreateFileA(LockFilePath, 0, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    b32 BuildSuccess = CompileCpp(BuildContext);
    DeleteFileA(LockFilePath);

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\handmade_hero\\win32_platform.cpp");

    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/Fmwin32_platform.map");

    AddLinkerFlags(BuildContext, "/incremental:no /subsystem:windows /opt:ref");
    AddLinkerFlags(BuildContext, "user32.lib gdi32.lib winmm.lib");

    SetOuputBinaryPath(BuildContext, "\\win32_platform.exe");

    BuildSuccess = BuildSuccess && CompileCpp(BuildContext);
    return BuildSuccess;
}

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

b32 BuildLint(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\build\\lint.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    if
    (
        (BuildContext->argc >= 3) &&
        (strcmp(BuildContext->argv[2], "job_per_directory") == 0)
    )
    {
        AddCompilerFlags(BuildContext, "/DJOB_PER_DIRECTORY");
    }
    else
    {
        AddCompilerFlags(BuildContext, "/DJOB_PER_FILE");
    }


    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref Shlwapi.lib");

    SetOuputBinaryPath(BuildContext, "\\lint.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}

b32 BuildX86Kernel(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\boot_sector.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\boot_sector.img");

    b32 BuildSuccess = CompileAssembly(BuildContext);

    ClearBuildContext(BuildContext);

    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\kernel.s");
    AddCompilerFlags(BuildContext, "-f bin");
    SetOuputBinaryPath(BuildContext, "\\x86_kernel.img");

    BuildSuccess = BuildSuccess && CompileAssembly(BuildContext);

    ClearBuildContext(BuildContext);

    fat12_disk *Fat12Disk = Fat12CreateRamDisk();
    SetOuputBinaryPath(BuildContext, "\\x86_kernel.img");
    read_file_result KernelImageFile = ReadFileIntoMemory(BuildContext->OutputBinaryPath);
    AddFileToRootDirectory(Fat12Disk, KernelImageFile.FileMemory, KernelImageFile.Size, "kernel", "bin");
    FreeFileMemory(KernelImageFile.FileMemory);

    ClearBuildContext(BuildContext);

    SetOuputBinaryPath(BuildContext, "\\floppy.img");

    WriteFileFromMemory(BuildContext->OutputBinaryPath, Fat12Disk, sizeof(fat12_disk));
    FreeFileMemory(Fat12Disk);

    char SourceBinaryFilePath[1024];
    ZeroMemory(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath));
    StringCchCatA(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath), BuildContext->OutputDirectoryPath);
    StringCchCatA(SourceBinaryFilePath, ArrayCount(SourceBinaryFilePath), "\\boot_sector.img");

    BuildSuccess = BuildSuccess && WriteBinaryFileOverAnother(SourceBinaryFilePath, 0, BuildContext->OutputBinaryPath);
    return BuildSuccess;
}

b32 BuildCompilationTests(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\build\\compilation_tests.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\compilation_tests.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}

b32 BuildFat12Tests(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\drivers\\fat12\\test.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\fat12_tests.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}

b32 BuildX86KernelTests(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\x86_kernel\\test.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\x86_kernel_tests.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}