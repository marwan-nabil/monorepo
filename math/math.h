#pragma once

#define PI32 3.1415926535f
#define SQRT2 1.4142135623f
#define F32MAX FLT_MAX
#define F32MIN -FLT_MAX

struct v2
{
    f32 X;
    f32 Y;
};

struct v3
{
    f32 X;
    f32 Y;
    f32 Z;
};

struct v4
{
    f32 X;
    f32 Y;
    f32 Z;
    f32 W;
};

struct coordinate_set
{
    v3 X;
    v3 Y;
    v3 Z;
};