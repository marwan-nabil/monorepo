b32 BuildImguiDemo(build_context *BuildContext)
{
    if (BuildContext->EnvironmentInfo.argc < 3)
    {
        ConsolePrintColored
        (
            "ERROR: invalid number of arguments for build imgui_demo ...\n",
            BuildContext->EnvironmentInfo.ConsoleContext,
            FOREGROUND_RED
        );
        DisplayHelp();
        return FALSE;
    }

    AddCompilerSourceFile(BuildContext, "\\sources\\win32\\imgui\\imgui*.cpp");

    AddCompilerFlags(BuildContext, "/nologo /Zi /MD /utf-8");
    AddCompilerFlags(BuildContext, "/DUNICODE /D_UNICODE /DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018");
    SetCompilerIncludePath(BuildContext, "\\sources\\win32");

    AddLinkerFlags(BuildContext, "user32.lib Gdi32.lib dwmapi.lib");

    if (strcmp(BuildContext->EnvironmentInfo.argv[2], "opengl2") == 0)
    {
        AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\imgui\\main_opengl2.cpp");
        AddLinkerFlags(BuildContext, "opengl32.lib");
        SetLinkerOutputBinary(BuildContext, "\\imgui_demo_opengl2.exe");
    }
    else if (strcmp(BuildContext->EnvironmentInfo.argv[2], "dx11") == 0)
    {
        AddCompilerSourceFile(BuildContext, "\\sources\\win32\\demos\\imgui\\main_dx11.cpp");
        AddLinkerFlags(BuildContext, "d3d11.lib d3dcompiler.lib");
        SetLinkerOutputBinary(BuildContext, "\\imgui_demo_dx11.exe");
    }
    else
    {
        ConsoleSwitchColor(BuildContext->EnvironmentInfo.ConsoleContext, FOREGROUND_RED);
        printf("ERROR: invalid argument \"%s\" for build imgui_demo ...\n", BuildContext->EnvironmentInfo.argv[2]);
        ConsoleResetColor(BuildContext->EnvironmentInfo.ConsoleContext);
        DisplayHelp();
        return FALSE;
    }

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}