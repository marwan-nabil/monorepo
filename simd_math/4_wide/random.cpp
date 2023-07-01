inline u32_lane
XORShift32(lane_random_series *Series)
{
    u32_lane Result = Series->State;
    Result ^= Result << 13;
    Result ^= Result >> 17;
    Result ^= Result << 5;
    Series->State = Result;
    return Result;
}

inline f32_lane
RandomUnilateral(lane_random_series *Series)
{
    f32_lane Result = (f32_lane)XORShift32(Series) / LaneF32FromU32(U32MAX);
    return Result;
}

inline f32_lane
RandomBilateral(lane_random_series *Series)
{
    f32_lane Result = -1.0f + 2.0f * RandomUnilateral(Series);
    return Result;
}
