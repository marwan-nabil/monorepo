#pragma once

struct rendering_buffer
{
    void *Memory;
    u32 Width;
    u32 Height;
    u32 BytesPerPixel;
    u32 Pitch;
};

struct window_private_data
{
    BITMAPINFO *LocalBufferBitmapinfo;
    rendering_buffer *LocalRenderingBuffer;
    i32 *RunningState;
};

struct button_sample 
{
    b8 IsDown;
};

struct user_input_sample
{
    button_sample Up;
    button_sample Down;
    button_sample Left;
    button_sample Right;
};

struct color
{
    f32 A, R, G, B;
};

struct simulation_state
{
    b8 Up;
    b8 Down;
    b8 Left;
    b8 Right;
};

typedef f32 potential;

struct electric_point
{
    potential CurrentPotential;
    potential NextPotential;
};

struct electric_wire
{
    electric_point *A;
    electric_point *B;
    u32 InverseEquillibriumRate;
};