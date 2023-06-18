#pragma once

struct rendering_buffer
{
    void *Memory;
    u32 Width;
    u32 Height;
    u32 BytesPerPixel;
    u32 Pitch;
};

struct color
{
    f32 A, R, G, B;
};