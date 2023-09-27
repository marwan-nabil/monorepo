@echo off

.\build.exe build optimized

move /Y build.temp.exe build.exe
del build.obj