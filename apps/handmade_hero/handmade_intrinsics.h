#if !defined(HANDMADE_INTRINSICS_H)

#include <math.h>

inline i32
RoundF32ToI32(f32 Real)
{
    //TODO: make intrinsic
    i32 Result = (i32)roundf(Real);
    return Result;
}

inline u32
RoundF32ToU32(f32 Real)
{
    //TODO: make intrinsic
    u32 Result = (u32)roundf(Real);
    return Result;
}

inline i32
FloorF32ToI32(f32 Real)
{
    //TODO: make intrinsic
    i32 Result = (i32)floorf(Real);
    return Result;
}

inline i32
CeilingF32ToI32(f32 Real)
{
    //TODO: make intrinsic
    i32 Result = (i32)ceilf(Real);
    return Result;
}

inline i32
TruncateF32ToI32(f32 Real)
{
    //TODO: make intrinsic
    i32 Result = (i32)(Real);
    return Result;
}

inline u32
TruncateF32ToU32(f32 Real)
{
    //TODO: make intrinsic
    u32 Result = (u32)(Real);
    return Result;
}

inline f32
Sin(f32 Angle)
{
    return sinf(Angle);
}

inline f32
Cos(f32 Angle)
{
    return cosf(Angle);
}

inline f32
Atan2(f32 Y, f32 X)
{
    return (f32)atan2(Y, X);
}

struct bitscan_result
{
    b32 Found;
    u32 Index;
};

inline bitscan_result
FindLeastSignificantSetBit(u32 Value)
{
    bitscan_result Result = {};
#ifdef COMPILER_MSVC
    Result.Found = _BitScanForward((unsigned long *)&Result.Index, Value);
#else
    for (u32 Test = 0; Test < 32; Test++)
    {
        if (Value & (1 << Test))
        {
            Result.Index = Test;
            Result.Found = true;
            break;
        }
    }
#endif
    return Result;
}

inline f32
AbsoluteValue(f32 A)
{
    f32 Result = (f32)fabs(A);
    return Result;
}

inline u32 
RotateLeft(u32 Value, i32 Rotation)
{
#ifdef COMPILER_MSVC
    u32 Result = _rotl(Value, Rotation);
#else
    u32 Result = (Rotation > 0) ? 
        (Value << Rotation) | (Value >> (32 - Rotation)) :
        (Value >> -Rotation) | (Value << (32 + Rotation));
#endif
    return Result;
}

inline u32
RotateRight(u32 Value, i32 Rotation)
{
#ifdef COMPILER_MSVC
    u32 Result = _rotr(Value, Rotation);
#else
    u32 Result = (Rotation > 0) ? 
        (Value >> Rotation) | (Value << (32 - Rotation)) :
        (Value << -Rotation) | (Value >> (32 + Rotation));
#endif
    return Result;
}

inline i32
SignOf(i32 Value)
{
    i32 Result = (Value >= 0)? 1 : -1; 
    return Result;
}

#define HANDMADE_INTRINSICS_H
#endif