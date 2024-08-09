@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

if "%1"=="debug" (
    set optimization_flags=/Od /Z7
) else (
    set optimization_flags=/O2
)

set cc_flags=/nologo %optimization_flags% /Oi /FC /GR- /EHa-
set cc_flags=%cc_flags% /W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018
set cc_flags=%cc_flags% /D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS
set cc_flags=%cc_flags% /I%root_path%

set link_flags=/link /subsystem:console /incremental:no /opt:ref user32.lib shell32.lib

if not exist outputs\bootstrapper; mkdir outputs\bootstrapper
pushd outputs\bootstrapper
    cl %cc_flags%^
        %root_path%\sources\win32\tools\bootstrapper\bootstrapper.cpp^
        %root_path%\sources\win32\tools\build\actions\build_context.cpp^
        %root_path%\sources\win32\tools\build\actions\msvc.cpp^
        %root_path%\sources\win32\libraries\file_system\folders.cpp^
        %root_path%\sources\win32\libraries\shell\console.cpp^
        %root_path%\sources\win32\libraries\strings\path_handling.cpp^
        %root_path%\sources\win32\libraries\strings\string_list.cpp^
        %root_path%\sources\win32\libraries\system\processes.cpp^
        /Fe:bootstrapper.exe^
        %link_flags%
popd

outputs\bootstrapper\bootstrapper.exe %1

if exist outputs\build\build.exe (
    if not exist tools\build; mkdir tools\build
    copy outputs\build\build.exe tools\build\build.exe
) else (
    echo ERROR: build.exe was not built!
)
