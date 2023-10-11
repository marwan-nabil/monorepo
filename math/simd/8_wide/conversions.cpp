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

inline f32
F32FromF32Lane(f32_lane Value, u32 Index)
{
    f32 Result = 0;
    switch (Index)
    {
        case 0:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 0), 0);
        } break;

        case 1:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 0), 1);
        } break;

        case 2:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 0), 2);
        } break;

        case 3:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 0), 3);
        } break;

        case 4:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 1), 0);
        } break;

        case 5:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 1), 1);
        } break;

        case 6:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 1), 2);
        } break;

        case 7:
        {
            *(u32 *)&Result = _mm_extract_ps(_mm256_extractf128_ps(Value, 1), 3);
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

inline u32
U32FromU32Lane(u32_lane Value, u32 Index)
{
    u32 Result = 0;
    switch (Index)
    {
        case 0:
        {
            Result = _mm256_extract_epi32(Value, 0);
        } break;

        case 1:
        {
            Result = _mm256_extract_epi32(Value, 1);
        } break;

        case 2:
        {
            Result = _mm256_extract_epi32(Value, 2);
        } break;

        case 3:
        {
            Result = _mm256_extract_epi32(Value, 3);
        } break;

        case 4:
        {
            Result = _mm256_extract_epi32(Value, 4);
        } break;

        case 5:
        {
            Result = _mm256_extract_epi32(Value, 5);
        } break;

        case 6:
        {
            Result = _mm256_extract_epi32(Value, 6);
        } break;

        case 7:
        {
            Result = _mm256_extract_epi32(Value, 7);
        } break;
    }
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