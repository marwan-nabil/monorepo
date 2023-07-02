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

inline u32_lane
operator^(u32_lane A, u32_lane B)
{
    u32_lane Result = _mm_xor_si128(A, B);
    return Result;
}
