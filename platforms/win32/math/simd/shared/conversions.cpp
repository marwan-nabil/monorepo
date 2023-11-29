/******************************************/
/*                  f32                   */
/******************************************/
inline f32
F32FromF32Lane(f32_lane Value, u32 Index)
{
    f32 Result = ((f32 *)&Value)[Index];
    return Result;
}

/******************************************/
/*                  u32                   */
/******************************************/
inline u32
U32FromU32Lane(u32_lane Value, u32 Index)
{
    u32 Result = ((u32 *)&Value)[Index];
    return Result;
}

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