inline void
ConditionalAssign(u32_lane *Destination, u32_lane Source, u32_lane Mask)
{
    Mask = Mask? 0xFFFFFFFF : 0;
    *Destination = (~Mask & *Destination) | (Mask & Source);
}

inline void
ConditionalAssign(f32_lane *Destination, f32_lane Source, u32_lane Mask)
{
    ConditionalAssign((u32 *)Destination, *(u32 *)&Source, Mask);
}

inline u32
HorizontalAdd(u32_lane WideValue)
{
    u32 NarrowValue = WideValue;
    return NarrowValue;
}

inline f32
HorizontalAdd(f32_lane WideValue)
{
    f32 NarrowValue = WideValue;
    return NarrowValue;
}

inline b32
MaskIsAllZeroes(u32_lane Mask)
{
    if (Mask)
    {
        return false;
    }
    return true;
}
