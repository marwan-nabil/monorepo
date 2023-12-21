@echo off

set cc_flags=/O2

if "%1"=="debug" (
    set cc_flags=/Od /Z7
)

pushd ..\tools\build
    cl^
        /nologo %cc_flags% /Oi /FC /GR- /EHa-^
        /W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018^
        /D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS^
        /I..\..^
        ..\..\sources\win32\tools\build\*.cpp^
        ..\..\sources\win32\console\console.cpp^
        ..\..\sources\win32\file_system\*.cpp^
        ..\..\sources\win32\strings\path_handling.cpp^
        ..\..\sources\win32\system\processes.cpp^
        ..\..\sources\win32\fat12\*.cpp^
        /Fe:build.exe^
        /link /subsystem:console /incremental:no /opt:ref user32.lib shell32.lib

    del *.obj
popd