@echo off

if "%1"=="build" (
    if exist win32\tools\build\build.sln (
        start win32\tools\build\build.sln
    ) else (
        devenv tools\build\build.exe
    )
)

if "%1"=="refterm" (
    devenv build_output\refterm\refterm_debug_msvc.exe
)