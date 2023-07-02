/******************************************/
/*                  Addition              */
/******************************************/
inline u32_lane
operator+(u32_lane A, u32 B)
{
    u32_lane Result = A + U32LaneFromU32(B);
    return Result;
}

inline u32_lane
operator+(u32 A, u32_lane B)
{
    u32_lane Result = U32LaneFromU32(A) + B;
    return Result;
}

inline u32_lane &
operator+=(u32_lane &A, u32_lane B)
{
    A = A + B;
    return A;
}

inline u32_lane &
operator+=(u32_lane &A, u32 B)
{
    A = A + U32LaneFromU32(B);
    return A;
}

/******************************************/
/*                  Subtraction           */
/******************************************/
inline u32_lane
operator-(u32_lane A, u32 B)
{
    u32_lane Result = A - U32LaneFromU32(B);
    return Result;
}

inline u32_lane
operator-(u32 A, u32_lane B)
{
    u32_lane Result = U32LaneFromU32(A) - B;
    return Result;
}

inline u32_lane &
operator-=(u32_lane &A, u32_lane B)
{
    A = A - B;
    return A;
}

inline u32_lane &
operator-=(u32_lane &A, u32 B)
{
    A = A - U32LaneFromU32(B);
    return A;
}

/******************************************/
/*               Multiplication           */
/******************************************/
inline u32_lane
operator*(u32_lane A, u32 B)
{
    u32_lane Result = A * U32LaneFromU32(B);
    return Result;
}

inline u32_lane
operator*(u32 A, u32_lane B)
{
    u32_lane Result = U32LaneFromU32(A) * B;
    return Result;
}

inline u32_lane &
operator*=(u32_lane &A, u32_lane B)
{
    A = A * B;
    return A;
}

inline u32_lane &
operator*=(u32_lane &A, u32 B)
{
    A = A * U32LaneFromU32(B);
    return A;
}

/******************************************/
/*                  Division              */
/******************************************/
inline u32_lane
operator/(u32_lane A, u32 B)
{
    u32_lane Result = A / U32LaneFromU32(B);
    return Result;
}

inline u32_lane
operator/(u32 A, u32_lane B)
{
    u32_lane Result = U32LaneFromU32(A) / B;
    return Result;
}

inline u32_lane &
operator/=(u32_lane &A, u32_lane B)
{
    A = A / B;
    return A;
}

inline u32_lane &
operator/=(u32_lane &A, u32 B)
{
    A = A / U32LaneFromU32(B);
    return A;
}

/******************************************/
/*                  bitwise and           */
/******************************************/
inline u32_lane
operator&(u32_lane A, u32 B)
{
    u32_lane Result = A & U32LaneFromU32(B);
    return Result;
}

inline u32_lane
operator&(u32 A, u32_lane B)
{
    u32_lane Result = U32LaneFromU32(A) & B;
    return Result;
}

inline u32_lane &
operator&=(u32_lane &A, u32_lane B)
{
    A = A & B;
    return A;
}

inline u32_lane &
operator&=(u32_lane &A, u32 B)
{
    A = A & U32LaneFromU32(B);
    return A;
}

/******************************************/
/*                  bitwise or            */
/******************************************/
inline u32_lane
operator|(u32_lane A, u32 B)
{
    u32_lane Result = A | U32LaneFromU32(B);
    return Result;
}

inline u32_lane
operator|(u32 A, u32_lane B)
{
    u32_lane Result = U32LaneFromU32(A) | B;
    return Result;
}

inline u32_lane &
operator|=(u32_lane &A, u32_lane B)
{
    A = A | B;
    return A;
}

inline u32_lane &
operator|=(u32_lane &A, u32 B)
{
    A = A | U32LaneFromU32(B);
    return A;
}

/******************************************/
/*                  bitwise xor           */
/******************************************/
inline u32_lane
operator^(u32_lane A, u32 B)
{
    u32_lane Result = A ^ U32LaneFromU32(B);
    return Result;
}

inline u32_lane
operator^(u32 A, u32_lane B)
{
    u32_lane Result = U32LaneFromU32(A) ^ B;
    return Result;
}

inline u32_lane &
operator^=(u32_lane &A, u32_lane B)
{
    A = A ^ B;
    return A;
}

inline u32_lane &
operator^=(u32_lane &A, u32 B)
{
    A = A ^ U32LaneFromU32(B);
    return A;
}

/******************************************/
/*             logical operations         */
/******************************************/
inline u32_lane
operator||(u32_lane A, u32_lane B)
{
    u32_lane Result = A | B;
    return Result;
}

inline u32_lane
operator&&(u32_lane A, u32_lane B)
{
    u32_lane Result = A & B;
    return Result;
}

inline u32_lane
operator!(u32_lane A)
{
    u32_lane Result = ~A;
    return Result;
}

/******************************************/
/*             other operations           */
/******************************************/
inline void
ConditionalAssign(u32_lane *Destination, u32_lane Source, u32_lane Mask)
{
    *Destination = (~Mask & *Destination) | (Mask & Source);
}


// -----------------------------------------------------------


// TODO: implement these for u32_lane usig intrinsics
#if 0

inline u32_lane
Clamp(u32_lane Value, u32_lane Min, u32_lane Max)
{
	u32_lane Result = Value;
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
#endif