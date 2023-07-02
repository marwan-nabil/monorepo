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
