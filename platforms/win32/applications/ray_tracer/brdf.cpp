#include <Windows.h>
#include <intrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include "platforms\win32\libraries\base_types.h"
#include "platforms\win32\libraries\basic_defines.h"
#include "platforms\win32\libraries\file_system\files.h"
#include "platforms\win32\libraries\math\constants.h"
#include "platforms\win32\libraries\math\integers.h"
#include "platforms\win32\libraries\math\floats.h"
#include "platforms\win32\libraries\math\scalar_conversions.h"
#include "platforms\win32\libraries\math\transcendentals.h"
#include "platforms\win32\libraries\math\random.h"
#include "platforms\win32\libraries\math\vector2.h"
#include "platforms\win32\libraries\math\vector3.h"
#include "platforms\win32\libraries\math\vector4.h"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "platforms\win32\libraries\math\simd\1_wide\math.h"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "platforms\win32\libraries\math\simd\4_wide\math.h"
#elif (SIMD_NUMBEROF_LANES == 8)
#   include "platforms\win32\libraries\math\simd\8_wide\math.h"
#else
#   error "the defined SIMD_NUMBEROF_LANES is still not supported"
#endif // SIMD_NUMBEROF_LANES == 1

#include "platforms\win32\libraries\math\simd\shared\math.h"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "platforms\win32\libraries\math\simd\1_wide\conversions.h"
#   include "platforms\win32\libraries\math\simd\1_wide\integers.h"
#   include "platforms\win32\libraries\math\simd\1_wide\floats.h"
#   include "platforms\win32\libraries\math\simd\1_wide\assertions.h"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "platforms\win32\libraries\math\simd\4_wide\conversions.h"
#   include "platforms\win32\libraries\math\simd\4_wide\integers.h"
#   include "platforms\win32\libraries\math\simd\4_wide\floats.h"
#   include "platforms\win32\libraries\math\simd\4_wide\assertions.h"
#   include "platforms\win32\libraries\math\simd\4_wide\vector3.h"
#elif (SIMD_NUMBEROF_LANES == 8)
#   include "platforms\win32\libraries\math\simd\8_wide\conversions.h"
#   include "platforms\win32\libraries\math\simd\8_wide\integers.h"
#   include "platforms\win32\libraries\math\simd\8_wide\floats.h"
#   include "platforms\win32\libraries\math\simd\8_wide\assertions.h"
#   include "platforms\win32\libraries\math\simd\8_wide\vector3.h"
#else
#   error "the defined SIMD_NUMBEROF_LANES is still not supported"
#endif // SIMD_NUMBEROF_LANES == 1

#include "platforms\win32\libraries\math\simd\shared\conversions.h"
#include "platforms\win32\libraries\math\simd\shared\integers.h"
#include "platforms\win32\libraries\math\simd\shared\floats.h"
#include "platforms\win32\libraries\math\simd\shared\vector3.h"
#include "platforms\win32\libraries\math\simd\shared\random.h"

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