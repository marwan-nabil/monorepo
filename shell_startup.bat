@echo off

@REM add local scripts and tools to system path
set path=%cd%\scripts;%cd%\tools;%path%

@REM setup shell environment for x86_real/protected platform tools
call "scripts\environment\x86_tools.bat"

@REM setup MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist output; mkdir output
if not exist tools; mkdir tools
cd output