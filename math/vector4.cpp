inline v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
	v4 Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	Result.W = W;
	return Result;
}

inline v4
V4(v3 XYZ, f32 W)
{
	v4 Result;
	Result.X = XYZ.X;
	Result.Y = XYZ.Y;
	Result.Z = XYZ.Z;
	Result.W = W;
	return Result;
}
