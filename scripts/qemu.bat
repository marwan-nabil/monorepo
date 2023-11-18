@echo off

pushd %1
    qemu-system-x86_64 -drive format=raw,file=floppy.img
popd