@echo off

if not exist hdl; mkdir hdl
pushd hdl
    if "%1"=="compile" (
        iverilog -I ..\..\sources\hdl -o testbench.vvp ..\..\sources\hdl\testbench.v
    )

    if "%1"=="simulate" (
        vvp testbench.vvp
    )

    if "%1"=="visualize" (
        gtkwave -f demo.vcd
    )
popd