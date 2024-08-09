@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

if "%1"=="" (
    echo ERROR: no argument provided to debug.bat
    exit /b 1
)

if "%1"=="build" (
    if exist configuration\visual_studio\build\build.sln (
        start configuration\visual_studio\build\build.sln
    ) else if exist outputs\build\build.exe (
        devenv outputs\build\build.exe
    ) else if exist tools\build\build.exe (
        devenv tools\build\build.exe
    ) else (
        echo ERROR: build.exe doesn't exist.
    )
)

if "%1"=="bootstrapper" (
    devenv outputs\bootstrapper\bootstrapper.exe
)

if "%1"=="refterm" (
    devenv outputs\refterm\refterm_debug_msvc.exe
)