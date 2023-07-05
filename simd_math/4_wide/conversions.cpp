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
F32LaneFromF32(f32 A, f32 B, f32 C, f32 D)
{
    f32_lane Result = _mm_set_ps(A, B, C, D);
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

inline f32_lane
StaticCastF32LaneFromU32Lane(u32_lane Value)
{
    f32_lane Result = _mm_castsi128_ps(Value);
    return Result;
}

inline f32
F32FromF32Lane(f32_lane Value, u32 Index)
{
    f32 Result = 0;
    switch (Index)
    {
        case 0:
        {
            *(u32 *)&Result = _mm_extract_ps(Value, 0);
        } break;
        
        case 1:
        {
            *(u32 *)&Result = _mm_extract_ps(Value, 1);
        } break;
        
        case 2:
        {
            *(u32 *)&Result = _mm_extract_ps(Value, 2);
        } break;
        
        case 3:
        {
            *(u32 *)&Result = _mm_extract_ps(Value, 3);
        } break;
    }
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
U32LaneFromU32(u32 A, u32 B, u32 C, u32 D)
{
    u32_lane Result = _mm_set_epi32(A, B, C, D);
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

inline u32
U32FromU32Lane(u32_lane Value, u32 Index)
{
    u32 Result = 0;
    switch (Index)
    {
        case 0:
        {
            Result = _mm_extract_epi32(Value, 0);
        } break;
        
        case 1:
        {
            Result = _mm_extract_epi32(Value, 1);
        } break;
        
        case 2:
        {
            Result = _mm_extract_epi32(Value, 2);
        } break;
        
        case 3:
        {
            Result = _mm_extract_epi32(Value, 3);
        } break;
    }
    return Result; 
}