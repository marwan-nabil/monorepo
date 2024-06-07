@echo off

set root_path=%cd%\..
set cc_flags=/O2

if "%1"=="debug" (
    set cc_flags=/Od /Z7
)

pushd ..\tools\build
    cl^
        /nologo %cc_flags% /Oi /FC /GR- /EHa-^
        /W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018^
        /D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS^
        /I%root_path%^
        %root_path%\sources\win32\tools\build\*.cpp^
        %root_path%\sources\win32\tools\build\targets\*.cpp^
        %root_path%\sources\win32\tools\build\targets\hdl\*.cpp^
        %root_path%\sources\win32\tools\build\targets\win32\*.cpp^
        %root_path%\sources\win32\tools\build\targets\i686-elf\*.cpp^
        %root_path%\sources\win32\tools\build\helpers\*.cpp^
        %root_path%\sources\win32\libraries\shell\console.cpp^
        %root_path%\sources\win32\libraries\file_system\*.cpp^
        %root_path%\sources\win32\libraries\strings\path_handling.cpp^
        %root_path%\sources\win32\libraries\strings\string_list.cpp^
        %root_path%\sources\win32\libraries\system\processes.cpp^
        %root_path%\sources\win32\libraries\file_system\fat12\*.cpp^
        /Fe:build.exe^
        /link /subsystem:console /incremental:no /opt:ref user32.lib shell32.lib

    @REM del *.obj
popd