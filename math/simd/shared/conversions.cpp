/******************************************/
/*                  v2_lane               */
/******************************************/
inline v2_lane
V2LaneFromV2(v2 Value)
{
    v2_lane Result =
    {
        F32LaneFromF32(Value.X),
        F32LaneFromF32(Value.Y)
    };
    return Result;
}

/******************************************/
/*                  v3_lane               */
/******************************************/
inline v3_lane
V3LaneFromV3(v3 Value)
{
    v3_lane Result =
    {
        F32LaneFromF32(Value.X),
        F32LaneFromF32(Value.Y),
        F32LaneFromF32(Value.Z)
    };
    return Result;
}

inline v3
V3FromV3Lane(v3_lane Value, u32 Index)
{
    v3 Result =
    {
        F32FromF32Lane(Value.X, Index),
        F32FromF32Lane(Value.Y, Index),
        F32FromF32Lane(Value.Z, Index)
    };
    return Result;
}

/******************************************/
/*                  v4_lane               */
/******************************************/
inline v4_lane
V4LaneFromV4(v4 Value)
{
    v4_lane Result =
    {
        F32LaneFromF32(Value.X),
        F32LaneFromF32(Value.Y),
        F32LaneFromF32(Value.Z),
        F32LaneFromF32(Value.W)
    };
    return Result;
}