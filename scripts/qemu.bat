@echo off

pushd os
    qemu-system-x86_64 -drive format=raw,file=floppy.img
popd