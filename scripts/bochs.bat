@echo off

pushd os
    bochsdbg.exe -q -f ..\..\sources\i686-elf\floppy_image\bochs_config.txt
popd