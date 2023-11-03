@echo off

build compilation_tests && compilation_tests\compilation_tests.exe
build fat12_tests && fat12_tests\fat12_tests.exe
build x86_os
build x86_os_tests && x86_os_tests\x86_os_tests.exe