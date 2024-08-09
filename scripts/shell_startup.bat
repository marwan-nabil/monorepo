@echo off

@REM adding gcc cross compiler, binutils & dependencies to shell path
set path=%cd%\tools\cygwin\lib\gcc\x86_64-pc-cygwin\11;%path%
set path=%cd%\tools\cygwin\bin;%path%
set path=%cd%\tools\i686-elf\bin;%path%

@REM adding external tools to shell path
set path=%cd%\tools\nasm;%path%
set path=%cd%\tools\qemu;%path%
set path=%cd%\tools\bochs;%path%
set path=%cd%\tools\iverilog\bin;%path%
set path=%cd%\tools\iverilog\gtkwave\bin;%path%

@REM add local scripts and tools to shell path
set path=%cd%\tools\build;%path%
set path=%cd%\tools\lint;%path%
set path=%cd%\scripts;%path%

set root_path=%cd%

@REM setup MSVC & windows SDK environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
