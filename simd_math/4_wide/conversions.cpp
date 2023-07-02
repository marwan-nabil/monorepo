/******************************************/
/*                  floats                */
/******************************************/
inline f32_lane
F32LaneFromF32(f32 Value)
{
    f32_lane Result = _mm_set1_ps(Value);
    return Result;
}

inline f32_lane
F32LaneFromU32(u32 Value)
{
    f32_lane Result = _mm_set1_ps((f32)Value);
    return Result;
}

inline f32_lane
F32LaneFromU32Lane(u32_lane Value)
{
    f32_lane Result = _mm_cvtepi32_ps(Value);
    return Result;
}

/******************************************/
/*                  integers              */
/******************************************/
inline u32_lane
U32LaneFromU32(u32 Value)
{
    u32_lane Result = _mm_set1_epi32(Value);
    return Result;
}

inline u32_lane
U32LaneFromF32Lane(f32_lane Value)
{
    u32_lane Result = _mm_cvtps_epi32(Value);
    return Result;
}

inline u32_lane
StaticCastU32LaneFromF32Lane(f32_lane Value)
{
    u32_lane Result = _mm_castps_si128(Value);
    return Result;
}
