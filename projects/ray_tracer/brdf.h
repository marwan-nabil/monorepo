#pragma once

#define BRDF_RESOLUTION_THETA_HALF 90
#define BRDF_RESOLUTION_THETA_DIFF 90
#define BRDF_RESOLUTION_PHI_DIFF 360

struct brdf_table
{
    u32 TotalSampleCount;
    u32 Dimensions[3];
    f64 *Samples;
};