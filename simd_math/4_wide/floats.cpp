/******************************************/
/*             basic operations           */
/******************************************/
inline f32_lane 
operator+(f32_lane A, f32_lane B)
{
    f32_lane Result = _mm_add_ps(A, B);
    return Result;
}

inline f32_lane 
operator-(f32_lane A, f32_lane B)
{
    f32_lane Result = _mm_sub_ps(A, B);
    return Result;
}

inline f32_lane 
operator*(f32_lane A, f32_lane B)
{
    f32_lane Result = _mm_mul_ps(A, B);
    return Result;
}

inline f32_lane 
operator/(f32_lane A, f32_lane B)
{
    f32_lane Result = _mm_div_ps(A, B);
    return Result;
}

/******************************************/
/*         comparison operations          */
/******************************************/
inline u32_lane
operator<(f32_lane A, f32_lane B)
{
    u32_lane Result = _mm_castps_si128(_mm_cmplt_ps(A, B));
    return Result;
}

inline u32_lane
operator<=(f32_lane A, f32_lane B)
{
    u32_lane Result = _mm_castps_si128(_mm_cmple_ps(A, B));
    return Result;
}

inline u32_lane
operator>(f32_lane A, f32_lane B)
{
    u32_lane Result = _mm_castps_si128(_mm_cmpgt_ps(A, B));
    return Result;
}

inline u32_lane
operator>=(f32_lane A, f32_lane B)
{
    u32_lane Result = _mm_castps_si128(_mm_cmpge_ps(A, B));
    return Result;
}

inline u32_lane
operator==(f32_lane A, f32_lane B)
{
    u32_lane Result = _mm_castps_si128(_mm_cmpeq_ps(A, B));
    return Result;
}

inline u32_lane
operator!=(f32_lane A, f32_lane B)
{
    u32_lane Result = _mm_castps_si128(_mm_cmpneq_ps(A, B));
    return Result;
}

/******************************************/
/*             Common Functions           */
/******************************************/
inline f32_lane 
SquareRoot(f32_lane A)
{
    f32_lane Result = _mm_sqrt_ps(A);
    return Result;
}

inline f32
HorizontalAdd(f32_lane WideValue)
{
    f32 NarrowValue;

    f32_lane Result0 = _mm_hadd_ps(WideValue, F32LaneFromF32(0));
    f32_lane Result1 = _mm_hadd_ps(Result0, F32LaneFromF32(0));

    *(u32 *)&NarrowValue = _mm_extract_ps(Result1, 0);

    return NarrowValue;
}
