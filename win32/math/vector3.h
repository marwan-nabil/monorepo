#pragma once

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

struct coordinate_set
{
    v3 X;
    v3 Y;
    v3 Z;
};