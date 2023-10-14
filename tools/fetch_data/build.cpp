b32 BuildFetchData(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\tools\\fetch_data\\fetch_data.cpp");

    AddCompilerFlags(BuildContext, "/nologo /I.. /Z7 /FC /Od /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib winhttp.lib");

    SetOuputBinaryPath(BuildContext, "\\fetch_data.exe");

    b32 BuildSuccess = CompileCpp(BuildContext);
    return BuildSuccess;
}