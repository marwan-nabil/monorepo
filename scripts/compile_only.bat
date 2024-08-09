@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

set cc_flags_0=/nologo /Oi /FC /GR- /EHa- /O2
set cc_flags_1=/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /wd4996 /wd4018
set cc_flags_2=/D_CRT_SECURE_NO_WARNINGS /D_CRT_RAND_S /DENABLE_ASSERTIONS

if not exist outputs\compile_only_test; mkdir outputs\compile_only_test
pushd outputs\compile_only_test
    cl /c^
        %cc_flags_0%^
        %cc_flags_1%^
        %cc_flags_2%^
        /I%root_path%^
        %root_path%\sources\win32\tools\lint\build.cpp^
popd