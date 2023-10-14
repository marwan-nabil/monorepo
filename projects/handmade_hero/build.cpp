b32 BuildHandmadeHero(build_context *BuildContext)
{
    AddSourceFile(BuildContext, "\\projects\\handmade_hero\\game.cpp");

    char SharedCompilerFlags[1024] = {};
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/nologo /I.. /Zi /FC /Od /Oi /GR- /EHa- /Gm- /MTd ");
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 ");
    StringCchCatA(SharedCompilerFlags, ArrayCount(SharedCompilerFlags), "/DHANDMADE_WIN32=1 /DHANDMADE_SLOW=1 /DHANDMADE_INTERNAL=1 /DENABLE_ASSERTIONS ");

    AddCompilerFlags(BuildContext, SharedCompilerFlags);
    AddCompilerFlags(BuildContext, "/LD /Fmgame.map");

    AddLinkerFlags(BuildContext, "/incremental:no");
    AddLinkerFlags(BuildContext, "/EXPORT:GameGetSoundSamples /EXPORT:GameUpdateAndRender");

    char PdbLinkerFlag[1024] = {};
    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), "/PDB:game_");

    char RandomString[5] = {};
    u32 RandomNumber;
    rand_s(&RandomNumber);
    RandomNumber = (u32)((f32)RandomNumber / (f32)UINT_MAX * 9999.0f);
    StringCchPrintfA(RandomString, ArrayCount(RandomString), "%d", RandomNumber);

    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), RandomString);
    StringCchCatA(PdbLinkerFlag, ArrayCount(PdbLinkerFlag), ".pdb");

    AddLinkerFlags(BuildContext, PdbLinkerFlag);

    SetOuputBinaryPath(BuildContext, "\\game.dll");

    char LockFilePath[MAX_PATH] = {};
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