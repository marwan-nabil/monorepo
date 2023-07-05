/******************************************/
/*              constructors              */
/******************************************/
inline v3_lane 
V3Lane(f32_lane X, f32_lane Y, f32_lane Z)
{
    v3_lane Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    return Result;
}

inline v3_lane 
V3Lane(f32 X, f32 Y, f32 Z)
{
    v3_lane Result;
    Result.X = F32LaneFromF32(X);
    Result.Y = F32LaneFromF32(Y);
    Result.Z = F32LaneFromF32(Z);
    return Result;
}

inline v3_lane
V3Lane(v2_lane XY, f32_lane Z)
{
	v3_lane Result;
	Result.X = XY.X;
	Result.Y = XY.Y;
	Result.Z = Z;
	return Result;
}

/******************************************/
/*              Addition                  */
/******************************************/
inline v3_lane 
operator+(v3_lane A, v3_lane B)
{
    v3_lane Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return Result;
}

inline v3_lane &
operator+=(v3_lane &A, v3_lane B)
{
    A = A + B;
    return A;
}

/******************************************/
/*              subtraction               */
/******************************************/
inline v3_lane 
operator-(v3_lane A, v3_lane B)
{
    v3_lane Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return Result;
}

inline v3_lane &
operator-=(v3_lane &A, v3_lane B)
{
    A = A - B;
    return A;
}

inline v3_lane 
operator-(v3_lane A)
{
    v3_lane Result;
    Result.X = -A.X;
    Result.Y = -A.Y;
    Result.Z = -A.Z;
    return Result;
}

/******************************************/
/*              multiplication            */
/******************************************/
inline v3_lane
operator*(f32_lane A, v3_lane B)
{
    v3_lane Result;
    Result.X = A * B.X;
    Result.Y = A * B.Y;
    Result.Z = A * B.Z;
    return Result;
}

inline v3_lane 
operator*(v3_lane A, f32_lane B)
{
    v3_lane Result;
    Result.X = A.X * B;
    Result.Y = A.Y * B;
    Result.Z = A.Z * B;
    return Result;
}

inline v3_lane &
operator*=(v3_lane &A, f32_lane B)
{
    A = A * B;
    return A;
}

/******************************************/
/*                division                */
/******************************************/
inline v3_lane 
operator/(v3_lane A, f32_lane B)
{
    v3_lane Result;
    Result.X = A.X / B;
    Result.Y = A.Y / B;
    Result.Z = A.Z / B;
    return Result;
}

inline v3_lane &
operator/=(v3_lane &A, f32_lane B)
{
    A = A / B;
    return A;
}

/******************************************/
/*                bitwise                 */
/******************************************/
inline v3_lane 
operator&(u32_lane Mask, v3_lane Value)
{
    v3_lane Result =
    {
        Mask & Value.X,
        Mask & Value.Y,
        Mask & Value.Z
    };
    return Result;
}

inline v3_lane 
operator&(v3_lane Value, u32_lane Mask)
{
    v3_lane Result = Mask & Value;
    return Result;
}

/******************************************/
/*           vector operations            */
/******************************************/
inline f32_lane 
InnerProduct(v3_lane A, v3_lane B)
{
    f32_lane Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;
    return Result;
}

inline f32_lane 
LengthSquared(v3_lane A)
{
    f32_lane Result = InnerProduct(A, A);
    return Result;
}

inline f32_lane 
Length(v3_lane A)
{
    f32_lane Result = SquareRoot(LengthSquared(A));
    return Result;
}

inline v3_lane 
Normalize(v3_lane A)
{
    v3_lane Result = A / Length(A);
    return Result;
}

inline v3_lane
HadamardProduct(v3_lane A, v3_lane B)
{
    v3_lane Result = V3Lane(A.X * B.X, A.Y * B.Y, A.Z * B.Z);
    return Result;
}

inline v3_lane
CrossProduct(v3_lane A, v3_lane B)
{
    v3_lane Result;

    Result.X = A.Y * B.Z - A.Z * B.Y;
    Result.Y = A.Z * B.X - A.X * B.Z;
    Result.Z = A.X * B.Y - A.Y * B.X;

    return Result;
}

inline v3_lane
Lerp(v3_lane A, v3_lane B, f32_lane t)
{
    v3_lane Result = (1.0f - t) * A + t * B;
    return Result;
}

inline void
ConditionalAssign(v3_lane *Destination, v3_lane Source, u32_lane Mask)
{
    ConditionalAssign(&Destination->X, Source.X, Mask);
    ConditionalAssign(&Destination->Y, Source.Y, Mask);
    ConditionalAssign(&Destination->Z, Source.Z, Mask);
}

inline v3
HorizontalAdd(v3_lane WideValue)
{
    v3 Result = 
    {
        HorizontalAdd(WideValue.X),
        HorizontalAdd(WideValue.Y),
        HorizontalAdd(WideValue.Z)
    };

    return Result;
}

// -----------------------------------------------------------
// TODO: implement these for v3_lane usig intrinsics

#if 0

inline v3_lane
Clamp01(v3_lane A)
{
	v3_lane Result;
	Result.X = Clamp01(A.X);
	Result.Y = Clamp01(A.Y);
	Result.Z = Clamp01(A.Z);
	return Result;
}

#endif