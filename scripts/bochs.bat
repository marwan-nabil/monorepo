@echo off

pushd %1
    bochsdbg.exe -q -f ..\..\projects\low_level\%1\bochs_config.txt
popd