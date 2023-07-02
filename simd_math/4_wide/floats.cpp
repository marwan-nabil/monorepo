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
/*             logical comparison         */
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

/******************************************/
/*             other operations           */
/******************************************/
inline f32_lane 
SquareRoot(f32_lane A)
{
    f32_lane Result = _mm_sqrt_ps(A);
    return Result;
}

// TODO: implement later with intrinsics
#if 0

inline f32
Power(f32 Base, f32 Exponent)
{
	f32 Result = ;
	return Result;
}

#endif