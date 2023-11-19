@echo off

@REM add local scripts and tools to system path
set path=%cd%\scripts;%cd%\tools;%path%

@REM TODO: remove watcom completely
@REM watcom variables
set path=%cd%\tools\watcom\BINW;%path%
set path=%cd%\tools\watcom\BINNT;%path%
set WATCOM=%cd%\tools\watcom
set EDPATH=%cd%\tools\watcom\EDDAT
set WHTMLHELP=%cd%\tools\watcom\BINNT\HELP
set WIPFC=%cd%\tools\watcom\WIPFC

@REM adding tools to path
set path=%cd%\tools\nasm;%path%
set path=%cd%\tools\qemu;%path%
set path=%cd%\tools\bochs;%path%

@REM setup MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist output; mkdir output
if not exist tools; mkdir tools
cd output