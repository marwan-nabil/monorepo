/******************************************/
/*          arithmetic operations         */
/******************************************/
inline u32_lane 
operator+(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_add_epi32(A, B);
    return Result;
}

inline u32_lane 
operator-(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_sub_epi32(A, B);
    return Result;
}

/******************************************/
/*             bitwise operations         */
/******************************************/
inline u32_lane
operator&(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_and_si128(A, B);
    return Result;
}

inline u32_lane
operator|(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_or_si128(A, B);
    return Result;
}

inline u32_lane
operator^(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_xor_si128(A, B);
    return Result;
}

inline u32_lane
operator~(u32_lane A)
{
    u32_lane Result = _mm_xor_si128(A, _mm_set1_epi32(0xFFFFFFFF));
    return Result;
}

inline u32_lane 
operator<<(u32_lane Value, u32 Shift)
{
    u32_lane Result = _mm_slli_epi32(Value, Shift);
    return Result;
}

inline u32_lane 
operator>>(u32_lane Value, u32 Shift)
{
    u32_lane Result = _mm_srli_epi32(Value, Shift);
    return Result;
}

/******************************************/
/*           comparison operations        */
/******************************************/
inline u32_lane
operator<(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_cmplt_epi32(A, B);
    return Result;
}

inline u32_lane
operator>(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_cmpgt_epi32(A, B);
    return Result;
}

inline u32_lane
operator==(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_cmpeq_epi32(A, B);
    return Result;
}

/******************************************/
/*             other operations           */
/******************************************/
inline b32
MaskIsAllZeroes(u32_lane Mask)
{
    b32 Result = _mm_test_all_zeros(_mm_set1_epi32(0xFFFFFFFF), Mask);
    return Result;
}

// inline u32
// HorizontalAdd(u32_lane WideValue)
// {
//     u32 NarrowValue = 
//         U32FromU32Lane(WideValue, 0) + 
//         U32FromU32Lane(WideValue, 1) + 
//         U32FromU32Lane(WideValue, 2) + 
//         U32FromU32Lane(WideValue, 3);
//     return NarrowValue;
// }

inline u32
HorizontalAdd(u32_lane WideValue)
{
    u32 NarrowValue;

    u32_lane Result0 = _mm_hadd_epi32(WideValue, U32LaneFromU32(0));
    u32_lane Result1 = _mm_hadd_epi32(Result0, U32LaneFromU32(0));

    *(u32 *)&NarrowValue = _mm_extract_epi32(Result1, 0);

    return NarrowValue;
}
