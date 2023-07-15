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
    u32_lane Result = B + A;
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
    A = A + B;
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

inline u32_lane
operator-(u32_lane A)
{
    u32_lane Result = U32LaneFromU32(0) - A;
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
/*           comparison operations        */
/******************************************/
inline u32_lane
operator!=(u32_lane A, u32_lane B)
{
    u32_lane Result = ~(A == B);
    return Result;
}

/******************************************/
/*             logical operations         */
/******************************************/
inline u32_lane
operator||(u32_lane A, u32_lane B)
{
    AssertGoodMask(A, __LINE__);
    AssertGoodMask(B, __LINE__);
    u32_lane Result = A | B;
    return Result;
}

inline u32_lane
operator&&(u32_lane A, u32_lane B)
{
    AssertGoodMask(A, __LINE__);
    AssertGoodMask(B, __LINE__);
    u32_lane Result = A & B;
    return Result;
}

inline u32_lane
operator!(u32_lane A)
{
    AssertGoodMask(A, __LINE__);
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

inline u32_lane
Max(u32_lane A, u32_lane B)
{
    u32_lane Result;
    u32_lane ComparisonMask = (A > B);
    ConditionalAssign(&Result, A, ComparisonMask);
    ConditionalAssign(&Result, B, ~ComparisonMask);
    return Result;
}

inline u32_lane
Min(u32_lane A, u32_lane B)
{
    u32_lane Result;
    u32_lane ComparisonMask = (A < B);
    ConditionalAssign(&Result, A, ComparisonMask);
    ConditionalAssign(&Result, B, ~ComparisonMask);
    return Result;
}

inline u32_lane
Clamp(u32_lane Value, u32_lane Minimum, u32_lane Maximum)
{
	u32_lane Result = Value;
    Result = Max(Value, Minimum);
    Result = Min(Value, Maximum);
	return Result;
}
