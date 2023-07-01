#if 1

inline void
ConditionalAssign(lane_u32 *Destination, lane_u32 Source, lane_u32 Mask)
{
    Mask = Mask? 0xFFFFFFFF : 0;
    *Destination = (~Mask & *Destination) | (Mask & Source);
}

#else

inline void
ConditionalAssign(lane_u32 *Destination, lane_u32 Source, lane_u32 Mask)
{
    if (Mask)
    {
        *Destination = Source;
    }
}

#endif

inline void
ConditionalAssign(lane_f32 *Destination, lane_f32 Source, lane_u32 Mask)
{
    ConditionalAssign((u32 *)Destination, *(u32 *)&Source, Mask);
}

inline void
ConditionalAssign(lane_v3 *Destination, lane_v3 Source, lane_u32 Mask)
{
    ConditionalAssign(&Destination->X, Source.X, Mask);
    ConditionalAssign(&Destination->Y, Source.Y, Mask);
    ConditionalAssign(&Destination->Z, Source.Z, Mask);
}

inline u32
HorizontalAdd(lane_u32 WideValue)
{
    u32 NarrowValue = WideValue;
    return NarrowValue;
}

inline f32
HorizontalAdd(lane_f32 WideValue)
{
    f32 NarrowValue = WideValue;
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
