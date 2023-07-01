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