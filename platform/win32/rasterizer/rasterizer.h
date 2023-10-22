#pragma once

struct rendering_buffer
{
    BITMAPINFO Bitmapinfo;
    void *Memory;
    u32 Width;
    u32 Height;
    u32 BytesPerPixel;
    u32 Pitch;
};