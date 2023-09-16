@echo off

set path="%cd%\scripts";%path%
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist output; mkdir output
cd output

cl^
    /nologo /FC /Oi /GR- /EHa- ^
    /W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018^
    /D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S^
    ..\build\build.cpp^
    /link /subsystem:console /incremental:no /opt:ref user32.lib

del build.obj

cls