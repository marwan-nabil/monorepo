#pragma once

struct brdf_table
{
    u32 SampleCount;
    v3 *ColorSamples;
};