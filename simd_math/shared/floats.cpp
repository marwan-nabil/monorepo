/******************************************/
/*                  Addition              */
/******************************************/
inline f32_lane
operator+(f32_lane A, f32 B)
{
    f32_lane Result = A + F32LaneFromF32(B);
    return Result;
}

inline f32_lane
operator+(f32 A, f32_lane B)
{
    f32_lane Result = F32LaneFromF32(A) + B;
    return Result;
}

inline f32_lane &
operator+=(f32_lane &A, f32_lane B)
{
    A = A + B;
    return A;
}

inline f32_lane &
operator+=(f32_lane &A, f32 B)
{
    A = A + F32LaneFromF32(B);
    return A;
}

/******************************************/
/*                  Subtraction           */
/******************************************/
inline f32_lane
operator-(f32_lane A, f32 B)
{
    f32_lane Result = A - F32LaneFromF32(B);
    return Result;
}

inline f32_lane
operator-(f32 A, f32_lane B)
{
    f32_lane Result = F32LaneFromF32(A) - B;
    return Result;
}

inline f32_lane
operator-(f32_lane A)
{
    f32_lane Result = 0.0f - A;
    return Result;
}

inline f32_lane &
operator-=(f32_lane &A, f32_lane B)
{
    A = A - B;
    return A;
}

inline f32_lane &
operator-=(f32_lane &A, f32 B)
{
    A = A - F32LaneFromF32(B);
    return A;
}

/******************************************/
/*               Multiplication           */
/******************************************/
inline f32_lane
operator*(f32_lane A, f32 B)
{
    f32_lane Result = A * F32LaneFromF32(B);
    return Result;
}

inline f32_lane
operator*(f32 A, f32_lane B)
{
    f32_lane Result = F32LaneFromF32(A) * B;
    return Result;
}

inline f32_lane &
operator*=(f32_lane &A, f32_lane B)
{
    A = A * B;
    return A;
}

inline f32_lane &
operator*=(f32_lane &A, f32 B)
{
    A = A * F32LaneFromF32(B);
    return A;
}

/******************************************/
/*                  Division              */
/******************************************/
inline f32_lane
operator/(f32_lane A, f32 B)
{
    f32_lane Result = A / F32LaneFromF32(B);
    return Result;
}

inline f32_lane
operator/(f32 A, f32_lane B)
{
    f32_lane Result = F32LaneFromF32(A) / B;
    return Result;
}

inline f32_lane &
operator/=(f32_lane &A, f32_lane B)
{
    A = A / B;
    return A;
}

inline f32_lane &
operator/=(f32_lane &A, f32 B)
{
    A = A / F32LaneFromF32(B);
    return A;
}

/******************************************/
/*             logical comparison         */
/******************************************/
inline u32_lane
operator<(f32_lane A, f32 B)
{
    u32_lane Result = A < F32LaneFromF32(B);
    AssertGoodMask(Result, __LINE__);
    return Result;
}

inline u32_lane
operator<=(f32_lane A, f32 B)
{
    u32_lane Result = A <= F32LaneFromF32(B);
    AssertGoodMask(Result, __LINE__);
    return Result;
}

inline u32_lane
operator>(f32_lane A, f32 B)
{
    u32_lane Result = A > F32LaneFromF32(B);
    AssertGoodMask(Result, __LINE__);
    return Result;
}

inline u32_lane
operator>=(f32_lane A, f32 B)
{
    u32_lane Result = A >= F32LaneFromF32(B);
    AssertGoodMask(Result, __LINE__);
    return Result;
}

/******************************************/
/*                masking                 */
/******************************************/
inline f32_lane 
operator&(u32_lane Mask, f32_lane Value)
{
    u32_lane ResultU32Lane = Mask & *(u32_lane *)&Value;
    f32_lane Result = *(f32_lane *)&ResultU32Lane;
    return Result;
}

inline f32_lane 
operator&(f32_lane Value, u32_lane Mask)
{
    f32_lane Result = Mask & Value;
    return Result;
}

/******************************************/
/*             other operations           */
/******************************************/
inline f32_lane 
Square(f32_lane A)
{
    return A * A;
}

inline f32_lane 
Power(f32_lane A, f32 Exponent)
{
    f32_lane Result = F32LaneFromF32
    (
        Power(F32FromF32Lane(A, 3), Exponent),
        Power(F32FromF32Lane(A, 2), Exponent),
        Power(F32FromF32Lane(A, 1), Exponent),
        Power(F32FromF32Lane(A, 0), Exponent)
    );
    return Result;
}

inline f32_lane 
Power(f32 A, f32_lane Exponent)
{
    f32_lane Result = F32LaneFromF32
    (
        Power(A, F32FromF32Lane(Exponent, 3)),
        Power(A, F32FromF32Lane(Exponent, 2)),
        Power(A, F32FromF32Lane(Exponent, 1)),
        Power(A, F32FromF32Lane(Exponent, 0))
    );
    return Result;
}

inline f32_lane 
Power(f32_lane A, f32_lane Exponent)
{
    f32_lane Result = F32LaneFromF32
    (
        Power(F32FromF32Lane(A, 3), F32FromF32Lane(Exponent, 3)),
        Power(F32FromF32Lane(A, 2), F32FromF32Lane(Exponent, 2)),
        Power(F32FromF32Lane(A, 1), F32FromF32Lane(Exponent, 1)),
        Power(F32FromF32Lane(A, 0), F32FromF32Lane(Exponent, 0))
    );
    return Result;
}

inline void
ConditionalAssign(f32_lane *Destination, f32_lane Source, u32_lane Mask)
{
    ConditionalAssign((u32_lane *)Destination, *(u32_lane *)&Source, Mask);
}

inline f32_lane
Max(f32_lane A, f32_lane B)
{
    f32_lane Result;
    u32_lane ComparisonMask = (A >= B);
    ConditionalAssign(&Result, A, ComparisonMask);
    ConditionalAssign(&Result, B, ~ComparisonMask);
    return Result;
}

inline f32_lane
Min(f32_lane A, f32_lane B)
{
    f32_lane Result;
    u32_lane ComparisonMask = (A <= B);
    ConditionalAssign(&Result, A, ComparisonMask);
    ConditionalAssign(&Result, B, ~ComparisonMask);
    return Result;
}

inline f32_lane
Clamp(f32_lane Value, f32_lane Minimum, f32_lane Maximum)
{
	f32_lane Result = Value;
    Result = Max(Value, Minimum);
    Result = Min(Value, Maximum);
	return Result;
}

inline f32_lane
Clamp(f32_lane Value, f32 Min, f32 Max)
{
	f32_lane Result = Clamp(Value, F32LaneFromF32(Min), F32LaneFromF32(Max));
	return Result;
}

inline f32_lane
Clamp01(f32_lane Value)
{
	f32_lane Result = Clamp(Value, 0.0f, 1.0f);
    return Result;
}

inline f32_lane
TranslateLinearTosRGB(f32_lane Linear)
{
    Linear = Clamp(Linear, 0.0f, 1.0f);

	f32_lane sRGB = Linear * 12.92f;

    f32_lane Expression = 1.055f * Power(Linear, 1.0f/2.4f) - 0.055f;

    ConditionalAssign(&sRGB, Expression, Linear > 0.0031308f);

	return sRGB;
}

// -----------------------------------------------------------
// TODO: implement these for f32_lane usig intrinsics
#if 0

inline f32
SafeRatioN(f32 Dividend, f32 Divisor, f32 AltValue)
{
	f32 Result = AltValue;
	if (Divisor != 0)
	{
		Result = Dividend / Divisor;
	}
	return Result;
}

inline f32
SafeRatio0(f32 Dividend, f32 Divisor)
{
	return SafeRatioN(Dividend, Divisor, 0);
}

inline f32
SafeRatio1(f32 Dividend, f32 Divisor)
{
	return SafeRatioN(Dividend, Divisor, 1.0f);
}

inline f32
Lerp(f32 A, f32 B, f32 T)
{
	f32 Result = (1.0f - T) * A + T * B;
	return Result;
}

#endif