#pragma once

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


// the vertex format
struct vertex
{
    float X, Y, Z;
    float NX, NY, NZ;
    float U, V; // texture coordinates
};
#define MYFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
