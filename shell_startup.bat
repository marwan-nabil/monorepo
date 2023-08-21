@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if not exist output; mkdir output
cd output
cl ..\build\build.cpp
cls
