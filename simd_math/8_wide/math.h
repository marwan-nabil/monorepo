#pragma once

typedef __m256i u32_lane;
typedef __m256 f32_lane;

union v2_lane
{
	struct
	{
		__m256 X;
		__m256 Y;
	};
	__m256 E[2];
};

union v3_lane
{
	struct
	{
		__m256 X;
		__m256 Y;
		__m256 Z;
	};
	struct
	{
		__m256 Red;
		__m256 Green;
		__m256 Blue;
	};
	struct
	{
		v2_lane XY;
		__m256 Ignored0;
	};
	struct
	{
		__m256 Ignored1;
		v2_lane YZ;
	};
	__m256 E[3];
};