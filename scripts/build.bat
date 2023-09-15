@echo off

set warnings=-W4 -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4996
set defs=-DHANDMADE_WIN32=1 -DHANDMADE_SLOW=1 -DHANDMADE_INTERNAL=1
set shared_cc_flags=-nologo -Zi -FC -Od -Oi -GR- -EHa- -Gm- -MTd

if not exist W:\handmade_hero\build; mkdir W:\handmade_hero\build

pushd W:\handmade_hero\build
    del *.pdb > NUL 2> NUL
    
    echo "Waiting for PDB production by compiler, don't load the game dll." > compilation.lock

    cl %shared_cc_flags% -LD^
       -Fmhandmade.map^
       %warnings%^
       %defs%^
       W:\handmade_hero\code\handmade.cpp^
       /link /incremental:no^
       /EXPORT:GameGetSoundSamples /EXPORT:GameUpdateAndRender /PDB:handmade__%random%.pdb

    del compilation.lock

    cl %shared_cc_flags%^
       -Fmwin32_handmade.map^
       %warnings%^
       %defs%^
       W:\handmade_hero\code\win32_handmade.cpp^
       /link /incremental:no /subsystem:windows /opt:ref^
       user32.lib gdi32.lib winmm.lib
popd
