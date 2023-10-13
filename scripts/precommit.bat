@echo off

call bootstrap
build lint && lint
build compilation_tests && compilation_tests