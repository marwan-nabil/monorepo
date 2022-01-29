
i32 RoundF32ToI32(f32 Real)
{
    i32 Result = (i32)roundf(Real);
    return Result;
}

u32 RoundF32ToU32(f32 Real)
{
    u32 Result = (u32)roundf(Real);
    return Result;
}

i32 FloorF32ToI32(f32 Real)
{
    i32 Result = (i32)floorf(Real);
    return Result;
}

i32 CeilingF32ToI32(f32 Real)
{
    i32 Result = (i32)ceilf(Real);
    return Result;
}

i32 TruncateF32ToI32(f32 Real)
{
    i32 Result = (i32)(Real);
    return Result;
}

u32 TruncateF32ToU32(f32 Real)
{
    u32 Result = (u32)(Real);
    return Result;
}

f32 Square(f32 A)
{
    return A * A;
}

f32 SquareRoot(f32 A)
{
    f32 Result = sqrtf(A);
    return Result;
}