@echo off

build compilation_tests && compilation_tests
build fat12_tests && fat12_tests
build x86_kernel
build x86_kernel_tests && x86_kernel_tests