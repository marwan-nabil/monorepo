@echo off

build build non_optimized
build build optimized

build simulator optimized
build simulator non_optimized

build ray_tracer optimized 1_lane
build ray_tracer optimized 4_lanes

build ray_tracer non_optimized 1_lane
build ray_tracer non_optimized 4_lanes