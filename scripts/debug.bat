@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

if "%1"=="build" (
    if exist sources\win32\tools\build\build.sln (
        start sources\win32\tools\build\build.sln
    ) else (
        devenv outputs\build\build.exe
    )
)

if "%1"=="bootstrapper" (
    if exist sources\win32\tools\build\bootstrapper.sln (
        start sources\win32\tools\build\bootstrapper.sln
    ) else (
        devenv outputs\bootstrapper\bootstrapper.exe
    )
)

if "%1"=="refterm" (
    devenv outputs\refterm\refterm_debug_msvc.exe
)