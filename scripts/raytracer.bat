@echo off

build ray_tracer optimized 4_lanes && ray_tracer_4.exe test_4_lanes.bmp && test_4_lanes.bmp
build ray_tracer optimized 1_lane && ray_tracer_1.exe test_1_lane.bmp && test_1_lane.bmp