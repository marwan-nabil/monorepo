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

inline f32 
Square(f32 A)
{
    return A * A;
}

inline f32 
SquareRoot(f32 A)
{
    f32 Result = sqrtf(A);
    return Result;
}

inline f32
Power(f32 Base, f32 Exponent)
{
	f32 Result = (f32)pow(Base, Exponent);
	return Result;
}

inline f32
RandomUnilateral()
{
    f32 Result = (f32)rand() / RAND_MAX;
    return Result;
}

inline f32
RandomBilateral()
{
    f32 Result = -1.0f + 2.0f * RandomUnilateral();
    return Result;
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