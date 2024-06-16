@echo off

if "%1"=="build" (
    if exist configuration\win32\tools\build\build.sln (
        start configuration\win32\tools\build\build.sln
    ) else (
        devenv tools\build\build.exe
    )
)

if "%1"=="refterm" (
    devenv outputs\refterm\refterm_debug_msvc.exe
)