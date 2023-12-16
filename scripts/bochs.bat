@echo off

pushd os
    bochsdbg.exe -q -f ..\..\configs\bochs.txt
popd