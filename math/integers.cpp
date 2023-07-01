inline i32
RoundF32ToI32(f32 Real)
{
    i32 Result = (i32)roundf(Real);
    return Result;
}

inline u32 
RoundF32ToU32(f32 Real)
{
    u32 Result = (u32)roundf(Real);
    return Result;
}

inline i32 
FloorF32ToI32(f32 Real)
{
    i32 Result = (i32)floorf(Real);
    return Result;
}

inline i32 
CeilingF32ToI32(f32 Real)
{
    i32 Result = (i32)ceilf(Real);
    return Result;
}

inline i32 
TruncateF32ToI32(f32 Real)
{
    i32 Result = (i32)(Real);
    return Result;
}

inline u32 
TruncateF32ToU32(f32 Real)
{
    u32 Result = (u32)(Real);
    return Result;
}

inline u32
Clamp(u32 Value, u32 Min, u32 Max)
{
	u32 Result = Value;
	if (Result < Min)
	{
		Result = Min;
	}
	else if (Result > Max)
	{
		Result = Max;
	}
	return Result;
}