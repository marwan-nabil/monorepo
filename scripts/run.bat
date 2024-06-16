@echo off

set root_path=%cd%

if "%1"=="verilog_demo" (
    pushd outputs\verilog_demo
        vvp testbench.vvp
        start gtkwave -f demo.vcd
    popd
)

if "%1"=="uart_app" (
    pushd outputs\uart_app
        vvp uart_app.vvp
        start gtkwave -f uart_app.vcd
    popd
)

if "%1"=="os" (
    pushd outputs\os
        start qemu-system-x86_64 -drive format=raw,file=floppy.img
    popd
)

if "%1"=="os_debug" (
    pushd outputs\os
        start bochsdbg -q -f %root_path%\configuration\i686-elf\bochs.txt
    popd
)