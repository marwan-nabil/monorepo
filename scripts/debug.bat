@echo off

if "%1"=="build" (
    devenv ..\tools\build\build.exe
)

if "%1"=="refterm" (
    devenv refterm\refterm_debug_msvc.exe
)