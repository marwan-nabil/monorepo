#pragma once

typedef __m128i u32_lane;
typedef __m128 f32_lane;

union v2_lane
{
	struct
	{
		__m128 X;
		__m128 Y;
	};
	__m128 E[2];
};

union v3_lane
{
	struct
	{
		__m128 X;
		__m128 Y;
		__m128 Z;
	};
	struct
	{
		__m128 Red;
		__m128 Green;
		__m128 Blue;
	};
	struct
	{
		v2_lane XY;
		__m128 Ignored0;
	};
	struct
	{
		__m128 Ignored1;
		v2_lane YZ;
	};
	__m128 E[3];
};