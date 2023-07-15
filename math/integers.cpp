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

inline u32
AbsoluteValue(i32 A)
{
	if (A < 0)
	{
		return -A;
	}
	else
	{
		return A;
	}
}