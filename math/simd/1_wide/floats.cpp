inline f32_lane
GatherF32Implementation(void *Base, u32 Stride, u32_lane Indices)
{
    u32 *IndexPointer = (u32 *)&Indices;
    f32_lane Result = F32LaneFromF32
    (
        *(f32 *)((u8 *)Base + IndexPointer[0] * Stride)
    );
    return Result;
}