@echo off

if not %cd% == %root_path% (
    echo ERROR: calling a script from outside the repository root path.
    exit /b 1
)

if "%1"=="build" (
    build build_tests
    build_output\build_tests\build_tests.exe
)

if "%1"=="fat12" (
    build fat12_tests
    build_output\fat12_tests\fat12_tests.exe
)

if "%1"=="ray_tracer" (
    build ray_tracer 8_lanes && build_output\ray_tracer\ray_tracer_8.exe build_output\ray_tracer\test_8_lanes.bmp && build_output\ray_tracer\test_8_lanes.bmp
    build ray_tracer 4_lanes && build_output\ray_tracer\ray_tracer_4.exe build_output\ray_tracer\test_4_lanes.bmp && build_output\ray_tracer\test_4_lanes.bmp
    build ray_tracer 1_lane && build_output\ray_tracer\ray_tracer_1.exe build_output\ray_tracer\test_1_lane.bmp && build_output\ray_tracer\test_1_lane.bmp
)