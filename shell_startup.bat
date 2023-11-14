@echo off

@REM add external tools to system path
set path=C:\Program Files\NASM;%path%
set path=C:\Program Files\qemu;%path%
set path=C:\Program Files\Bochs-2.7;%path%

@REM add local scripts to system path
set path=%cd%\scripts;%cd%\tools;%path%

@REM setup watcom environment
SET PATH=C:\Watcom\BINW;%PATH%
SET PATH=C:\Watcom\BINNT;%PATH%
SET WATCOM=C:\Watcom
SET EDPATH=C:\Watcom\EDDAT
SET WHTMLHELP=C:\Watcom\BINNT\HELP
SET WIPFC=C:\Watcom\WIPFC

@REM setup MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist output; mkdir output
if not exist tools; mkdir tools
cd tools
call bootstrap
cd ..\output
cls