@echo off

.\build.exe build
move /Y build.temp.exe build.exe
del build.obj