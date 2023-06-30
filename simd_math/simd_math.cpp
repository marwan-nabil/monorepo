
inline void
ConditionalAssign(lane_u32 *LeftHandSide, lane_u32 RightHandSide, lane_u32 Mask)
{
    if (Mask)
    {
        *LeftHandSide = RightHandSide;
    }
}

inline void
ConditionalAssign(lane_f32 *LeftHandSide, lane_f32 RightHandSide, lane_u32 Mask)
{
    if (Mask)
    {
        *LeftHandSide = RightHandSide;
    }
}

inline void
ConditionalAssign(lane_v3 *LeftHandSide, lane_v3 RightHandSide, lane_u32 Mask)
{
    if (Mask)
    {
        *LeftHandSide = RightHandSide;
    }
}

inline void
ConditionalAssign(lane_v2 *LeftHandSide, lane_v2 RightHandSide, lane_u32 Mask)
{
    if (Mask)
    {
        *LeftHandSide = RightHandSide;
    }
}

inline u32
HorizontalAdd(lane_u32 WideValue)
{
    u32 NarrowValue = WideValue;
    return NarrowValue;
}

inline lane_f32
RandomBilateralLane(random_series *Series)
{
    return RandomBilateral(Series);
}

inline b32
MaskIsAllZeroes(lane_u32 Mask)
{
    if (Mask)
    {
        return false;
    }
    return true;
}

inline lane_f32
Max(lane_f32 A, lane_f32 B)
{
    if (A > B)
    {
        return A;
    }
    else
    {
        return B;
    }
}
