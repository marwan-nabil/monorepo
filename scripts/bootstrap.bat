@echo off

@REM rebuilds the build system itself

.\build.exe build optimized

move /Y build.temp.exe build.exe

del build.obj