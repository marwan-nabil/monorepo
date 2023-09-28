inline v3_lane
V3Lane(f32_lane X, f32_lane Y, f32_lane Z)
{
    v3_lane Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    return Result;
}

inline v3_lane
operator&(u32_lane A, v3_lane B)
{
    if (A == 0xFFFFFFFF)
    {
        return B;
    }
    else
    {
        Assert(A == 0);
        return V3(0, 0, 0);
    }
}

inline v3
HorizontalAdd(v3_lane WideValue)
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
ConditionalAssign(v3_lane *Destination, v3_lane Source, u32_lane Mask)
{
    ConditionalAssign(&Destination->X, Source.X, Mask);
    ConditionalAssign(&Destination->Y, Source.Y, Mask);
    ConditionalAssign(&Destination->Z, Source.Z, Mask);
}

inline v3_lane
GatherV3Implementation(v3 *Base, u32 Stride, u32_lane Indices)
{
    v3_lane Result =
    {
        GatherF32Implementation(&Base->X, Stride, Indices),
        GatherF32Implementation(&Base->Y, Stride, Indices),
        GatherF32Implementation(&Base->Z, Stride, Indices)
    };
    return Result;
}