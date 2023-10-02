typedef b32 (*build_function_type)();

struct map_item
{
    char *TargetName;
    build_function_type *BuildFunction;
};

b32 BuildBuild()
{
    StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/nologo /Z7 /Oi /FC /Od /GR- /EHa- ");
    StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018 ");
    StringCchCatA(CompilerFlags, ArrayCount(CompilerFlags), "/D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS ");

    StringCchCatA(SourcesString, ArrayCount(SourcesString), RootDirectoryPath);
    StringCchCatA(SourcesString, ArrayCount(SourcesString), "\\build\\build.cpp");

    StringCchCatA(LinkerFlags, ArrayCount(LinkerFlags), "/subsystem:console /incremental:no /opt:ref user32.lib ");

    StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), OutputDirectoryPath);
    StringCchCatA(OutputBinaryPath, ArrayCount(OutputBinaryPath), "\\build.temp.exe");

    b32 BuildSuccess = CompileCpp(CompilerFlags, SourcesString, OutputBinaryPath, LinkerFlags);
    return BuildSuccess;
}

map_item BuildTargetMap[] =
{
    {
        "build",
        &BuildBuild
    },
};