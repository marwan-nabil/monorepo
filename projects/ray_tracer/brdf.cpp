inline void
LoadBrdfDataForMaterial(char *FileName, material *Material)
{
    read_file_result TableFile = ReadFileIntoMemory(FileName);
    FreeFileMemory(TableFile.FileMemory);
}

inline v3_lane
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