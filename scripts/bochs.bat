@echo off

pushd os
    bochsdbg.exe -q -f ..\..\projects\os\bochs_config.txt
popd