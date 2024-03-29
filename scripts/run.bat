@echo off

if "%1"=="verilog_demo" (
    pushd verilog_demo
        vvp testbench.vvp
        gtkwave -f demo.vcd
    popd
)

if "%1"=="uart_app" (
    pushd uart_app
        vvp uart_app.vvp
        gtkwave -f uart_app.vcd
    popd
)

if "%1"=="os" (
    pushd os
        @REM bochsdbg.exe -q -f ..\..\configuration\bochs.txt
        qemu-system-x86_64 -drive format=raw,file=floppy.img
    popd
)