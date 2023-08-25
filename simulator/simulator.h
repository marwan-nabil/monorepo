#pragma once

struct window_private_data
{
    rendering_buffer *LocalRenderingBuffer;
    b32 *RunningState;
};

struct simulation_state
{
    b8 Up;
    b8 Down;
    b8 Left;
    b8 Right;
};

struct electric_point
{
    f32 CurrentPotential;
    f32 NextPotential;
};

struct electric_wire
{
    electric_point *A;
    electric_point *B;
    u32 InverseEquillibriumRate;
};