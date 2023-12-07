#pragma once

struct brdf_sample
{
    f32 ThetaIn;
    f32 PhiIn;
    f32 ThetaOut;
    f32 PhiOut;
    v3 Color;
};

struct brdf_table
{
    u32 SampleCount;
    brdf_sample *Samples;
};