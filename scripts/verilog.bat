@echo off

pushd verilog_demo
    vvp testbench.vvp
    gtkwave -f demo.vcd
popd