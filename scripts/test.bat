@echo off

if "%1"=="build" (
    build build_tests
    outputs\build_tests\build_tests.exe
)

if "%1"=="fat12" (
    build fat12_tests
    outputs\fat12_tests\fat12_tests.exe
)

if "%1"=="ray_tracer" (
    build ray_tracer 8_lanes && outputs\ray_tracer\ray_tracer_8.exe outputs\ray_tracer\test_8_lanes.bmp && outputs\ray_tracer\test_8_lanes.bmp
    build ray_tracer 4_lanes && outputs\ray_tracer\ray_tracer_4.exe outputs\ray_tracer\test_4_lanes.bmp && outputs\ray_tracer\test_4_lanes.bmp
    build ray_tracer 1_lane && outputs\ray_tracer\ray_tracer_1.exe outputs\ray_tracer\test_1_lane.bmp && outputs\ray_tracer\test_1_lane.bmp
)