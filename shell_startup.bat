@echo off

set path=C:\Program Files\NASM;%path%
set path=C:\Program Files\qemu;%path%
set path=C:\Program Files\Bochs-2.7;%path%
set path=%cd%\scripts;%path%

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist output; mkdir output
cd output
call bootstrap
cls