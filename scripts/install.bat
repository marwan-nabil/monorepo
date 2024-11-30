@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

if "%1"=="build" (
    if exist outputs\build\build.exe (
        if not exist tools\build; mkdir tools\build
        copy outputs\build\build.exe tools\build\build.exe
    ) else (
        echo ERROR: build.exe doesn't exist.
    )
)

if "%1"=="bootstrapper" (
    if exist outputs\bootstrapper\bootstrapper.exe (
        if not exist tools\bootstrapper; mkdir tools\bootstrapper
        copy outputs\bootstrapper\bootstrapper.exe tools\bootstrapper\bootstrapper.exe
    ) else (
        echo ERROR: bootstrapper.exe doesn't exist.
    )
)