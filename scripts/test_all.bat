@echo off

build compilation_tests && compilation_tests\compilation_tests.exe
build fat12_tests && fat12_tests\fat12_tests.exe
build x86_kernel
build x86_kernel_tests && x86_kernel_tests\x86_kernel_tests.exe