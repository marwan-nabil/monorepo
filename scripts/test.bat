@echo off

if "%1"=="compilation" (
    build compilation_tests
    compilation_tests\compilation_tests.exe
)

if "%1"=="fat12" (
    build fat12_tests
    fat12_tests\fat12_tests.exe
)