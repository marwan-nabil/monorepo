@echo off

pushd %1
    bochsdbg.exe -q -f ..\..\projects\%1\bochs_config.txt
popd