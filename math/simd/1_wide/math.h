#pragma once

typedef f32 f32_lane;
typedef u32 u32_lane;
typedef i32 i32_lane;
typedef v2 v2_lane;
typedef v3 v3_lane;
typedef v4 v4_lane;
typedef random_series random_series_lane;

#define GatherF32(BasePointer, Member, Indices) \
    GatherF32Implementation(&((BasePointer)->Member), sizeof(*(BasePointer)), Indices)

#define GatherV3(BasePointer, Member, Indices) \
    GatherV3Implementation(&((BasePointer)->Member), sizeof(*(BasePointer)), Indices)