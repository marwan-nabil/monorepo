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