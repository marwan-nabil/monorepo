inline lane_v3
operator&(lane_u32 A, lane_v3 B)
{
    if (A == 0xFFFFFFFF)
    {
        return B;
    }
    else
    {
        return V3(0, 0, 0);
    }
}

inline v3
HorizontalAdd(lane_v3 WideValue)
{
    v3 NarrowValue = 
    {
        HorizontalAdd(WideValue.X),
        HorizontalAdd(WideValue.Y),
        HorizontalAdd(WideValue.Z)
    };
    return NarrowValue;
}

inline void
ConditionalAssign(lane_v3 *Destination, lane_v3 Source, lane_u32 Mask)
{
    ConditionalAssign(&Destination->X, Source.X, Mask);
    ConditionalAssign(&Destination->Y, Source.Y, Mask);
    ConditionalAssign(&Destination->Z, Source.Z, Mask);
}