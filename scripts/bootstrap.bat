@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

set cc_flags=/O2
if "%1"=="debug" (
    set cc_flags=/Od /Z7
)

if not exist tools\build; mkdir tools\build
pushd tools\build
    cl^
        /nologo %cc_flags% /Oi /FC /GR- /EHa-^
        /W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018^
        /D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS^
        /I%root_path%^
        %root_path%\win32\tools\build\actions\build_context.cpp^
        %root_path%\win32\tools\build\actions\msvc.cpp^
        %root_path%\win32\tools\build\bootstrapper.cpp^
        %root_path%\win32\libraries\file_system\folders.cpp^
        %root_path%\win32\libraries\shell\console.cpp^
        %root_path%\win32\libraries\strings\path_handling.cpp^
        %root_path%\win32\libraries\strings\string_list.cpp^
        %root_path%\win32\libraries\system\processes.cpp^
        /Fe:bootstrapper.exe^
        /link /subsystem:console /incremental:no /opt:ref user32.lib shell32.lib

    del /Q *.obj *.lib *.exp
popd

if "%1"=="debug" (
    call bootstrapper.exe debug
) else (
    call bootstrapper.exe
)

if exist build_output\build\build.exe (
    if exist tools\build\build.exe; del /Q tools\build\build.exe
    copy build_output\build\build.exe tools\build
)

if exist build_output\build\build.pdb (
    if exist tools\build\build.pdb; del /Q tools\build\build.pdb
    copy build_output\build\build.pdb tools\build
)