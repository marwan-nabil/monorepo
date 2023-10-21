b32 BuildCompilationTests(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\tools\\compilation_tests\\compilation_tests.cpp");

    AddCompilerFlags(BuildContext, "/nologo /I.. /Z7 /FC /Oi /GR- /EHa- /MTd /fp:fast /fp:except-");
    AddCompilerFlags(BuildContext, "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 /wd4127");
    AddCompilerFlags(BuildContext, "/DENABLE_ASSERTIONS /D_CRT_SECURE_NO_WARNINGS");

    AddLinkerFlags(BuildContext, "/subsystem:console /incremental:no /opt:ref user32.lib");

    SetOuputBinaryPath(BuildContext, "\\compilation_tests.exe");

    b32 BuildSuccess = CompileWithMSVC(BuildContext);
    return BuildSuccess;
}