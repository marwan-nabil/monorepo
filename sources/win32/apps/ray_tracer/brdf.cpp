#include <Windows.h>
#include <intrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include "sources\win32\base_types.h"
#include "sources\win32\basic_defines.h"
#include "sources\win32\file_system\files.h"
#include "sources\win32\math\constants.h"
#include "sources\win32\math\integers.h"
#include "sources\win32\math\floats.h"
#include "sources\win32\math\scalar_conversions.h"
#include "sources\win32\math\transcendentals.h"
#include "sources\win32\math\random.h"
#include "sources\win32\math\vector2.h"
#include "sources\win32\math\vector3.h"
#include "sources\win32\math\vector4.h"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "sources\win32\math\simd\1_wide\math.h"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "sources\win32\math\simd\4_wide\math.h"
#elif (SIMD_NUMBEROF_LANES == 8)
#   include "sources\win32\math\simd\8_wide\math.h"
#else
#   error "the defined SIMD_NUMBEROF_LANES is still not supported"
#endif // SIMD_NUMBEROF_LANES == 1

#include "sources\win32\math\simd\shared\math.h"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "sources\win32\math\simd\1_wide\conversions.h"
#   include "sources\win32\math\simd\1_wide\integers.h"
#   include "sources\win32\math\simd\1_wide\floats.h"
#   include "sources\win32\math\simd\1_wide\assertions.h"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "sources\win32\math\simd\4_wide\conversions.h"
#   include "sources\win32\math\simd\4_wide\integers.h"
#   include "sources\win32\math\simd\4_wide\floats.h"
#   include "sources\win32\math\simd\4_wide\assertions.h"
#   include "sources\win32\math\simd\4_wide\vector3.h"
#elif (SIMD_NUMBEROF_LANES == 8)
#   include "sources\win32\math\simd\8_wide\conversions.h"
#   include "sources\win32\math\simd\8_wide\integers.h"
#   include "sources\win32\math\simd\8_wide\floats.h"
#   include "sources\win32\math\simd\8_wide\assertions.h"
#   include "sources\win32\math\simd\8_wide\vector3.h"
#else
#   error "the defined SIMD_NUMBEROF_LANES is still not supported"
#endif // SIMD_NUMBEROF_LANES == 1

#include "sources\win32\math\simd\shared\conversions.h"
#include "sources\win32\math\simd\shared\integers.h"
#include "sources\win32\math\simd\shared\floats.h"
#include "sources\win32\math\simd\shared\vector3.h"
#include "sources\win32\math\simd\shared\random.h"

#include "brdf.h"
#include "ray_tracer.h"

void LoadReflectionDataForMaterial(char *FileName, material *Material)
{
    read_file_result TableFile = ReadFileIntoMemory(FileName);
    FreeFileMemory(TableFile);
}

v3_lane
GetMaterialReflectionColor
(
    material *Materials, u32_lane MaterialIndex, u32_lane LaneMask,
    v3_lane Tangent, v3_lane BiTangent, v3_lane Normal,
    v3_lane Incoming, v3_lane Outgoing
)
{
    v3_lane HitMaterialReflectionColor = {};
    u32_lane MaterialBrdfTableMask = U32LaneFromU32(0);

    for (u32 SubElementIndex = 0; SubElementIndex < SIMD_NUMBEROF_LANES; SubElementIndex++)
    {
        material *SubElementMaterial = &Materials[U32FromU32Lane(MaterialIndex, SubElementIndex)];
        if (SubElementMaterial->BrdfTable)
        {
            ((u32 *)&MaterialBrdfTableMask)[SubElementIndex] = 0xFFFFFFFF;
        }
    }

    if (!MaskIsAllZeroes(MaterialBrdfTableMask))
    {
        // NOTE: BRDF table lookup
    }

    if (!MaskIsAllZeroes(~MaterialBrdfTableMask))
    {
        f32_lane CosineAttenuationFactor = Max(InnerProduct(Incoming, Normal), F32LaneFromF32(0));
        ConditionalAssign
        (
            &HitMaterialReflectionColor,
            CosineAttenuationFactor * (LaneMask & GatherV3(Materials, ReflectionColor, MaterialIndex)),
            ~MaterialBrdfTableMask
        );
    }

    return HitMaterialReflectionColor;
}