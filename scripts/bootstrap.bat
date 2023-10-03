@echo off

cl^
    /nologo /Oi /Z7 /FC /Od /GR- /EHa-^
    /W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018^
    /D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS^
    ..\build\build.cpp^
    /link /subsystem:console /incremental:no /opt:ref user32.lib

del build.obj
del build.pdb