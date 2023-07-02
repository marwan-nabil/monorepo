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

/******************************************/
/*                  integers              */
/******************************************/
inline u32_lane
U32LaneFromU32(u32 Value)
{
    return Value;
}

inline u32_lane
U32LaneFromF32Lane(f32_lane Value)
{
    return (u32_lane)Value;
}

inline u32_lane
StaticCastU32LaneFromF32Lane(f32_lane Value)
{
    return *(u32_lane *)&Value;
}

/******************************************/
/*                  v3_lane               */
/******************************************/
inline v3_lane
V3LaneFromV3(v3 Value)
{
    return Value;
}