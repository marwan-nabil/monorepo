static f64 DummyBrdfColorValue[] = {0, 0, 0};

static void
LoadNullBrdfTable(brdf_table *Result)
{
    Result->Dimensions[0] = 1;
    Result->Dimensions[1] = 1;
    Result->Dimensions[2] = 1;
    Result->Samples = DummyBrdfColorValue;
    Result->TotalSampleCount = 1;
}

static void
LoadBrdfTableFromFile(char *FileName, brdf_table *Result)
{
    FILE *File = fopen(FileName, "rb");
    if (!File)
    {
        fprintf(stderr, "unable to open MERL BRDF file: %s\n", FileName);
        return;
    }

    fread(Result->Dimensions, sizeof(Result->Dimensions), 1, File);
    Result->TotalSampleCount =
        Result->Dimensions[0] *
        Result->Dimensions[1] *
        Result->Dimensions[2];

    Assert
    (
        Result->TotalSampleCount ==
        (
            BRDF_RESOLUTION_THETA_HALF *
            BRDF_RESOLUTION_THETA_DIFF *
            BRDF_RESOLUTION_PHI_DIFF / 2
        )
    );

    u32 TotalReadSize = Result->TotalSampleCount * sizeof(f64) * 3;
    Result->Samples = (f64 *)malloc(TotalReadSize);

    fread(Result->Samples, TotalReadSize, 1, File);
    fclose(File);
}

inline u32 ThetaHalfIndex(f64 ThetaHalf)
{
    if (ThetaHalf <= 0.0f)
    {
        return 0;
    }

    f64 IndexF64 = SquareRoot(ThetaHalf / (0.5f * PI32)) * (f64)BRDF_RESOLUTION_THETA_HALF;
    u32 Index = Clamp(RoundF32ToU32((f32)IndexF64), 0, BRDF_RESOLUTION_THETA_HALF - 1);
    return Index;
}

inline u32 ThetaDiffIndex(f64 ThetaDiff)
{
    f64 IndexF64 = (ThetaDiff / (0.5f * PI32)) * (f64)BRDF_RESOLUTION_THETA_DIFF;
    u32 Index = Clamp(RoundF32ToU32((f32)IndexF64), 0, (BRDF_RESOLUTION_THETA_DIFF - 1));
    return Index;
}

inline u32 PhiDiffIndex(f64 PhiDiff)
{
    if (PhiDiff < 0.0f)
    {
        PhiDiff += PI32;
    }

    f64 IndexF64 = (PhiDiff / PI32) * (BRDF_RESOLUTION_PHI_DIFF / 2);
    u32 Index = Clamp(RoundF32ToU32((f32)IndexF64), 0, (BRDF_RESOLUTION_PHI_DIFF / 2 - 1));
    return Index;
}

inline v3
BrdfTableLookup(brdf_table *Table, v3 Tangent, v3 BiTangent, v3 Normal, v3 Incoming, v3 Outgoing)
{
    v3 Halfway = Normalize((Incoming + Outgoing) / 2.0f);

    f64 ThetaIncoming = AcosF64(InnerProduct(Incoming, Normal));
    f64 ThetaHalf = AcosF64(InnerProduct(Halfway, Normal));
    f64 ThetaDiff = ThetaIncoming - ThetaHalf;

    f64 PhiIncoming = Atan2F64
    (
        InnerProduct(Incoming, BiTangent),
        InnerProduct(Incoming, Tangent)
    );
    f64 PhiHalf = Atan2F64
    (
        InnerProduct(Halfway, BiTangent),
        InnerProduct(Halfway, Tangent)
    );
    f64 PhiDiff = PhiIncoming - PhiHalf;

#if 0
    f64 ThetaIncomingDegrees = ThetaIncoming / PI32 * 180.0f;
    f64 ThetaHalfDegrees = ThetaHalf / PI32 * 180.0f;
    f64 ThetaDiffDegrees = ThetaDiff / PI32 * 180.0f;

    f64 PhiIncomingDegrees = PhiIncoming / PI32 * 180.0f;
    f64 PhiHalfDegrees = PhiHalf / PI32 * 180.0f;
    f64 PhiDiffDegrees = PhiDiff / PI32 * 180.0f;
#endif

    u32 SampleIndex =
        PhiDiffIndex(PhiDiff) +
        ThetaDiffIndex(ThetaDiff) * BRDF_RESOLUTION_PHI_DIFF / 2 +
        ThetaHalfIndex(ThetaHalf) * BRDF_RESOLUTION_PHI_DIFF / 2 * BRDF_RESOLUTION_THETA_DIFF;

    u32 GreenSamplesStartOffset =
        BRDF_RESOLUTION_THETA_HALF *
        BRDF_RESOLUTION_THETA_DIFF *
        BRDF_RESOLUTION_PHI_DIFF / 2;

    u32 BlueSamplesStartOffset =
        BRDF_RESOLUTION_THETA_HALF *
        BRDF_RESOLUTION_THETA_DIFF *
        BRDF_RESOLUTION_PHI_DIFF;

    v3 Result = V3
    (
        (f32)Table->Samples[SampleIndex] * (1.0 / 1500.0),
        (f32)Table->Samples[GreenSamplesStartOffset + SampleIndex] * (1.15 / 1500.0),
        (f32)Table->Samples[BlueSamplesStartOffset + SampleIndex] * (1.66 / 1500.0)
    );
    return Result;
}

inline v3_lane
BrdfTableLookup
(
    material *Materials, u32_lane MaterialIndex,
    v3_lane Tangent, v3_lane BiTangent, v3_lane Normal,
    v3_lane Incoming, v3_lane Outgoing
)
{
    v3_lane Result = {};

    for (u32 SubElementIndex = 0; SubElementIndex < SIMD_NUMBEROF_LANES; SubElementIndex++)
    {
        brdf_table *BrdfTable = &Materials[U32FromU32Lane(MaterialIndex, SubElementIndex)].BrdfTable;
        v3 TangentSingle = V3FromV3Lane(Tangent, SubElementIndex);
        v3 BiTangentSingle = V3FromV3Lane(BiTangent, SubElementIndex);
        v3 NormalSingle = V3FromV3Lane(Normal, SubElementIndex);
        v3 IncomingSingle = V3FromV3Lane(Incoming, SubElementIndex);
        v3 OutgoingSingle = V3FromV3Lane(Outgoing, SubElementIndex);

        v3 Color = BrdfTableLookup(BrdfTable, TangentSingle, BiTangentSingle, NormalSingle, IncomingSingle, OutgoingSingle);

        ((f32 *)&Result.X)[SubElementIndex] = Color.X;
        ((f32 *)&Result.Y)[SubElementIndex] = Color.Y;
        ((f32 *)&Result.Z)[SubElementIndex] = Color.Z;
    }

    return Result;
}

// inline v3_lane
// BrdfTableLookup
// (
//     material *Materials, u32_lane MaterialIndex,
//     v3_lane SurfaceTangent, v3_lane SurfaceBiTangent, v3_lane SurfaceNormal,
//     v3_lane ViewDirection, v3_lane LightDirection
// )
// {
//     v3_lane Result;

//     v3_lane HalfDirection = Normalize(0.5f * (ViewDirection + LightDirection));

//     v3_lane LightVector = Normalize
//     (
//         V3Lane
//         (
//             InnerProduct(LightDirection, SurfaceTangent),
//             InnerProduct(LightDirection, SurfaceBiTangent),
//             InnerProduct(LightDirection, SurfaceNormal)
//         )
//     );

//     v3_lane HalfVector = Normalize
//     (
//         V3Lane
//         (
//             InnerProduct(HalfDirection, SurfaceTangent),
//             InnerProduct(HalfDirection, SurfaceBiTangent),
//             InnerProduct(HalfDirection, SurfaceNormal)
//         )
//     );

//     coordinate_set_lane NewCoordinateSet;
//     NewCoordinateSet.Z = HalfVector;
//     NewCoordinateSet.Y = Normalize(CrossProduct(NewCoordinateSet.Z, SurfaceTangent));
//     NewCoordinateSet.X = Normalize(CrossProduct(NewCoordinateSet.Y, NewCoordinateSet.Z));

// #if 1
//     // NOTE: most probably the opposite is true
//     v3_lane NewLightVector = V3Lane
//     (
//         InnerProduct(NewCoordinateSet.X, LightVector),
//         InnerProduct(NewCoordinateSet.Y, LightVector),
//         InnerProduct(NewCoordinateSet.Z, LightVector)
//     );
// #else
//     v3_lane NewLightVector = V3Lane
//     (
//         InnerProduct(LightVector, NewCoordinateSet.X),
//         InnerProduct(LightVector, NewCoordinateSet.Y),
//         InnerProduct(LightVector, NewCoordinateSet.Z)
//     );
// #endif

//     for (u32 SubElementIndex = 0; SubElementIndex < SIMD_NUMBEROF_LANES; SubElementIndex++)
//     {
//         brdf_table *BrdfTable = &Materials[U32FromU32Lane(MaterialIndex, SubElementIndex)].BrdfTable;

//         f32 HalfVectorTheta = Acos(V3FromV3Lane(HalfVector, SubElementIndex).Z);

//         f32 NewLightVectorTheta = Acos(F32FromF32Lane(NewLightVector.Z, SubElementIndex));
//         f32 NewLightVectorPhi = Atan2
//         (
//             F32FromF32Lane(NewLightVector.Y, SubElementIndex),
//             F32FromF32Lane(NewLightVector.X, SubElementIndex)
//         );

//         if (NewLightVectorPhi < 0)
//         {
//             NewLightVectorPhi += PI32;
//         }

//         // ---------------------
//         f32 F0 = SquareRoot(Clamp01(HalfVectorTheta / (0.5f * PI32)));
//         u32 U0 = RoundF32ToU32((BrdfTable->Dimensions[0] - 1) * F0);

//         f32 F1 = Clamp01(NewLightVectorTheta / (0.5f * PI32));
//         u32 U1 = RoundF32ToU32((BrdfTable->Dimensions[1] - 1) * F1);

//         f32 F2 = Clamp01(NewLightVectorPhi / PI32);
//         u32 U2 = RoundF32ToU32((BrdfTable->Dimensions[2] - 1) * F2);

//         u32 BrdfTableIndex =
//             U2 +
//             U1 * BrdfTable->Dimensions[2] +
//             U0 * BrdfTable->Dimensions[2] * BrdfTable->Dimensions[1];
//         Assert(BrdfTableIndex < BrdfTable->TotalSampleCount);
//         // ---------------------

//         v3 Color = BrdfTable->Samples[BrdfTableIndex];

//         ((f32 *)&Result.X)[SubElementIndex] = Color.X;
//         ((f32 *)&Result.Y)[SubElementIndex] = Color.Y;
//         ((f32 *)&Result.Z)[SubElementIndex] = Color.Z;
//     }

//     return Result;
// }