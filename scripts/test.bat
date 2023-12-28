@echo off

if "%1"=="compilation" (
    build compilation_tests
    compilation_tests\compilation_tests.exe
)

if "%1"=="fat12" (
    build fat12_tests
    fat12_tests\fat12_tests.exe
)

if "%1"=="ray_tracer" (
    build ray_tracer 8_lanes && ray_tracer\ray_tracer_8.exe ray_tracer\test_8_lanes.bmp && ray_tracer\test_8_lanes.bmp
    build ray_tracer 4_lanes && ray_tracer\ray_tracer_4.exe ray_tracer\test_4_lanes.bmp && ray_tracer\test_4_lanes.bmp
    build ray_tracer 1_lane && ray_tracer\ray_tracer_1.exe ray_tracer\test_1_lane.bmp && ray_tracer\test_1_lane.bmp
)

if "%1"=="verilog_demo" (
    build verilog_demo
    pushd verilog_demo
        vvp testbench.vvp
        gtkwave -f demo.vcd
    popd
)

if "%1"=="os" (
    build os
    pushd os
        @REM bochsdbg.exe -q -f ..\..\configs\bochs.txt
        qemu-system-x86_64 -drive format=raw,file=floppy.img
    popd
)