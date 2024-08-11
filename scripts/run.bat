@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

if "%1"=="verilog_demo" (
    pushd build_output\verilog_demo
        vvp testbench.vvp
        start gtkwave -f demo.vcd
    popd
)

if "%1"=="uart_app" (
    pushd build_output\uart_app
        vvp uart_app.vvp
        start gtkwave -f uart_app.vcd
    popd
)

if "%1"=="os" (
    pushd build_output\os
        start qemu-system-x86_64 -drive format=raw,file=floppy.img
    popd
)

if "%1"=="os_debug" (
    pushd build_output\os
        start bochsdbg -q -f %root_path%\i686-elf\bochs.txt
    popd
)