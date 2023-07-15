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