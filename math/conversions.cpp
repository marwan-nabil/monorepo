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
PackColor(v4 Color)
{
    u32 Result =
    (
        (RoundF32ToU32(Color.Alpha * 255.0f) << 24) |
        (RoundF32ToU32(Color.Red * 255.0f) << 16) |
        (RoundF32ToU32(Color.Green * 255.0f) << 8) |
        RoundF32ToU32(Color.Blue * 255.0f)
    );
    return Result;
}