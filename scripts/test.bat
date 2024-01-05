@echo off

if "%1"=="build" (
    build build_tests
    build_tests\build_tests.exe
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