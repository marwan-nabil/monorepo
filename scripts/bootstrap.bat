@echo off

set cc_flags=/O2

if "%1"=="debug" (
    set cc_flags=/Od /Z7
)

pushd ..\tools
    cl^
        /nologo %cc_flags% /Oi /FC /GR- /EHa-^
        /W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018^
        /D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS^
        /I..\^
        ..\projects\tools\build\build.cpp^
        /link /subsystem:console /incremental:no /opt:ref user32.lib shell32.lib

    del build.obj
popd