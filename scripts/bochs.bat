@echo off

pushd x86_protected_os
    bochsdbg.exe -q -f ..\..\projects\low_level\os_protected\bochs_config.txt
popd