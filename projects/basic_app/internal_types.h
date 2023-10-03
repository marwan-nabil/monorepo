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
    b8 DoubleSpeed;

    u32 SelectedDataPointIndex;

    u32 DataPointCount;
    f32 *DataValues;
    v4 *DataColors;
};

struct user_input
{
    b8 Up;
    b8 Down;
    b8 Left;
    b8 Right;

    b8 Shift;
    b8 ExitSignal;

    u32 Number;
};