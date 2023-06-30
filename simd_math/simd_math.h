#pragma once

typedef f32 lane_f32;
typedef u32 lane_u32;
typedef v2 lane_v2;
typedef v3 lane_v3;

// union lane_v2
// {
// 	struct
// 	{
// 		lane_f32 X;
// 		lane_f32 Y;
// 	};
// 	lane_f32 E[2];
// };

// union lane_v3
// {
// 	struct
// 	{
// 		lane_f32 X;
// 		lane_f32 Y;
// 		lane_f32 Z;
// 	};
// 	struct
// 	{
// 		lane_f32 Red;
// 		lane_f32 Green;
// 		lane_f32 Blue;
// 	};
// 	struct
// 	{
// 		lane_v2 XY;
// 		lane_f32 Ignored0;
// 	};
// 	struct
// 	{
// 		lane_f32 Ignored1;
// 		lane_v2 YZ;
// 	};
// 	lane_f32 E[3];
// };