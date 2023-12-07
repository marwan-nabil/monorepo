#pragma once

struct render_piece
{
    loaded_bitmap *Bitmap;
    v4 Color;
    v3 Offset;
    f32 EntityJumpZCoefficient;
    v2 Dimensions;
};

struct render_peice_group
{
    u32 Count;
    render_piece Peices[8];
};