@echo off

build ray_tracer 8_lanes
@rem && ray_tracer\ray_tracer_8.exe ray_tracer\test_8_lanes.bmp && ray_tracer\test_8_lanes.bmp
build ray_tracer 4_lanes
@rem && ray_tracer\ray_tracer_4.exe ray_tracer\test_4_lanes.bmp && ray_tracer\test_4_lanes.bmp
build ray_tracer 1_lane
@rem && ray_tracer\ray_tracer_1.exe ray_tracer\test_1_lane.bmp && ray_tracer\test_1_lane.bmp