@echo off

@REM adding gcc cross compiler, binutils & dependencies to shell path
set path=%cd%\tools\cygwin\lib\gcc\x86_64-pc-cygwin\11;%path%
set path=%cd%\tools\cygwin\bin;%path%
set path=%cd%\tools\i686-elf\bin;%path%

@REM adding external tools to shell path
set path=%cd%\tools\nasm;%path%
set path=%cd%\tools\qemu;%path%
set path=%cd%\tools\bochs;%path%

@REM add local scripts and tools to shell path
set path=%cd%\tools\build;%path%
set path=%cd%\tools\build_dynamic;%path%
set path=%cd%\tools\lint;%path%
set path=%cd%\scripts;%path%

@REM setup MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist outputs; mkdir outputs
if not exist tools\build; mkdir tools\build
if not exist tools\build_dynamic; mkdir tools\build_dynamic
cd outputs