@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

if "%1"=="build" (
    if exist win32\tools\build\build.sln (
        start win32\tools\build\build.sln
    ) else (
        devenv tools\build\build.exe
    )
)

if "%1"=="bootstrapper" (
    if exist win32\tools\build\bootstrapper.sln (
        start win32\tools\build\bootstrapper.sln
    ) else (
        devenv tools\build\bootstrapper.exe
    )
)

if "%1"=="refterm" (
    devenv build_output\refterm\refterm_debug_msvc.exe
)