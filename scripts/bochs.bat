@echo off

pushd os
    bochsdbg.exe -q -f ..\..\configs\bochs_config.txt
popd