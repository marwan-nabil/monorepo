/******************************************/
/*                  floats                */
/******************************************/
inline f32_lane
F32LaneFromF32(f32 Value)
{
    f32_lane Result = _mm256_set1_ps(Value);
    return Result;
}

inline f32_lane
F32LaneFromF32(f32 A, f32 B, f32 C, f32 D, f32 E, f32 F, f32 G, f32 H)
{
    f32_lane Result = _mm256_set_ps(A, B, C, D, E, F, G, H);
    return Result;
}

inline f32_lane
F32LaneFromU32(u32 Value)
{
    f32_lane Result = _mm256_set1_ps((f32)Value);
    return Result;
}

inline f32_lane
F32LaneFromU32Lane(u32_lane Value)
{
    f32_lane Result = _mm256_cvtepi32_ps(Value);
    return Result;
}

inline f32_lane
StaticCastU32LaneToF32Lane(u32_lane Value)
{
    f32_lane Result = _mm256_castsi256_ps(Value);
    return Result;
}

/******************************************/
/*                  integers              */
/******************************************/
inline u32_lane
U32LaneFromU32(u32 Value)
{
    u32_lane Result = _mm256_set1_epi32(Value);
    return Result;
}

inline u32_lane
U32LaneFromU32(u32 A, u32 B, u32 C, u32 D, u32 E, u32 F, u32 G, u32 H)
{
    u32_lane Result = _mm256_set_epi32(A, B, C, D, E, F, G, H);
    return Result;
}

inline u32_lane
StaticCastF32LaneToU32Lane(f32_lane Value)
{
    u32_lane Result = _mm256_castps_si256(Value);
    return Result;
}

inline u32_lane
MaskFromBoolean(u32_lane Value)
{
    u32_lane Result = _mm256_xor_si256
    (
        _mm256_cmpeq_epi32(Value, U32LaneFromU32(0)),
        _mm256_set1_epi32(0xFFFFFFFF)
    );
    return Result;
}