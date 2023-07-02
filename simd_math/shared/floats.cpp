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
/*             other operations           */
/******************************************/
inline f32_lane 
Square(f32_lane A)
{
    return A * A;
}

inline void
ConditionalAssign(f32_lane *Destination, f32_lane Source, u32_lane Mask)
{
    ConditionalAssign((u32_lane *)Destination, *(u32_lane *)&Source, Mask);
}


// -----------------------------------------------------------
// TODO: implement these for f32_lane usig intrinsics
#if 0

inline f32
HorizontalAdd(f32_lane WideValue)
{
    f32 NarrowValue = WideValue;
    return NarrowValue;
}

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

inline f32
Clamp(f32 Value, f32 Min, f32 Max)
{
	f32 Result = Value;
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

inline f32
Clamp01(f32 Value)
{
	return Clamp(Value, 0.0f, 1.0f);
}

inline f32
Max(f32 A, f32 B)
{
    if (A > B)
    {
        return A;
    }
    else
    {
        return B;
    }
}

inline f32
TranslateLinearTosRGB(f32 Linear)
{
	if (Linear < 0)
	{
		Linear = 0;
	}
	if (Linear > 1)
	{
		Linear = 1;
	}

	f32 sRGB = Linear * 12.92f;
	if (Linear > 0.0031308f)
	{
		sRGB = 1.055f * Power(Linear, 1.0f/2.4f) - 0.055f;
	}

	return sRGB;
}

#endif