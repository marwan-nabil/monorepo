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