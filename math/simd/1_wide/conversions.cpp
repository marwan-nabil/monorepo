/******************************************/
/*                  floats                */
/******************************************/
inline f32_lane
F32LaneFromF32(f32 Value)
{
    return Value;
}

inline f32_lane
F32LaneFromU32(u32 Value)
{
    return (f32_lane)Value;
}

inline f32_lane
F32LaneFromU32Lane(u32_lane Value)
{
    return (f32_lane)Value;
}

inline f32_lane
StaticCastU32LaneToF32Lane(u32_lane Value)
{
    return *(f32_lane *)&Value;
}

inline f32
F32FromF32Lane(f32_lane Value, u32 Index)
{
    return (f32)Value;
}

/******************************************/
/*                  integers              */
/******************************************/
inline u32_lane
U32LaneFromU32(u32 Value)
{
    return Value;
}

inline u32
U32FromU32Lane(u32_lane Value, u32 Index)
{
    return Value;
}

inline u32_lane
StaticCastF32LaneToU32Lane(f32_lane Value)
{
    return *(u32_lane *)&Value;
}


inline u32_lane
MaskFromBoolean(u32_lane Value)
{
    if (Value)
    {
        return 0xFFFFFFFF;
    }
    else
    {
        return 0;
    }
}

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