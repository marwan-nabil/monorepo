@echo off

if "%1"=="verilog_demo" (
    if not exist verilog_demo; build verilog_demo
    pushd verilog_demo
        vvp testbench.vvp
        gtkwave -f demo.vcd
    popd
)

if "%1"=="os" (
    if not exist os; build os
    pushd os
        @REM bochsdbg.exe -q -f ..\..\configs\bochs.txt
        qemu-system-x86_64 -drive format=raw,file=floppy.img
    popd
)