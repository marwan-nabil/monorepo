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
        qemu-system-x86_64 -drive format=raw,file=floppy.img
    popd
)

if "%1"=="os_debug" (
    pushd os
        bochsdbg.exe -q -f ..\..\configuration\i686-elf\bochs.txt
    popd
)