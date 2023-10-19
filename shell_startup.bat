@echo off

@REM add external tools to system path
set path=C:\Program Files\NASM;%path%
set path=C:\Program Files\qemu;%path%
set path=C:\Program Files\Bochs-2.7;%path%

@REM add local scripts to system path
set path=%cd%\scripts;%path%

@REM setup compiler environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist output; mkdir output
cd output
call bootstrap
cls