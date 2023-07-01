#pragma once

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