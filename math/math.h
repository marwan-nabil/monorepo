#pragma once

#define PI32 3.1415926535f
#define SQRT2 1.4142135623f
#define F32MAX FLT_MAX
#define F32MIN -FLT_MAX
#define U32MAX UINT32_MAX
#define I32MAX INT32_MAX
#define I32MIN INT32_MIN

union v2
{
	struct
	{
		f32 X;
		f32 Y;
	};
	f32 E[2];
};

union v3
{
	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
	};
	struct
	{
		f32 Red;
		f32 Green;
		f32 Blue;
	};
	struct
	{
		v2 XY;
		f32 Ignored0;
	};
	struct
	{
		f32 Ignored1;
		v2 YZ;
	};
	f32 E[3];
};

union v4
{
	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
		f32 W;
	};
	struct
	{
		f32 Red;
		f32 Green;
		f32 Blue;
		f32 Alpha;
	};
	f32 E[4];
};

struct coordinate_set
{
    v3 X;
    v3 Y;
    v3 Z;
};

struct rectangle2
{
    v2 MinPoint;
    v2 MaxPoint;
};

struct rectangle3
{
	v3 MinPoint;
	v3 MaxPoint;
};