@echo off

set path=%cd%\scripts;C:\Program Files\NASM;C:\Program Files\qemu;%path%
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist output; mkdir output
cd output

cls