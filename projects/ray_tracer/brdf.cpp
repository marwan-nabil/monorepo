static v3 DummyBrdfColorValue = {0, 0, 0};

static void
LoadNullBrdf(brdf_table *Result)
{
    Result->SampleCount[0] = 1;
    Result->SampleCount[1] = 1;
    Result->SampleCount[2] = 1;
    Result->Samples = &DummyBrdfColorValue;
    Result->TotalSampleCount = 1;
}

static void
LoadMerlBrdfFile(char *FileName, brdf_table *Result)
{
    FILE *File = fopen(FileName, "rb");
    if (!File)
    {
        fprintf(stderr, "unable to open MERL BRDF file: %s\n", FileName);
        return;
    }

    fread(Result->SampleCount, sizeof(Result->SampleCount), 1, File);
    Result->TotalSampleCount =
        Result->SampleCount[0] *
        Result->SampleCount[1] *
        Result->SampleCount[2];

    u32 TotalReadSize = Result->TotalSampleCount * sizeof(f64) * 3;
    u32 ResultTableSize = Result->TotalSampleCount * sizeof(v3);

    f64 *TemporarySamples = (f64 *)malloc(TotalReadSize);
    Result->Samples = (v3 *)malloc(ResultTableSize);

    fread(TemporarySamples, TotalReadSize, 1, File);

    for (u32 SampleIndex = 0; SampleIndex < Result->TotalSampleCount; SampleIndex++)
    {
        Result->Samples[SampleIndex] = V3
        (
            (f32)TemporarySamples[3 * SampleIndex + 0],
            (f32)TemporarySamples[3 * SampleIndex + 1],
            (f32)TemporarySamples[3 * SampleIndex + 2]
        );
    }

    fclose(File);
    free(TemporarySamples);
}

inline v3_lane
BrdfTableLookup
(
    material *Materials, u32_lane MaterialIndex,
    v3_lane SurfaceTangent, v3_lane SurfaceBiTangent, v3_lane SurfaceNormal,
    v3_lane ViewDirection, v3_lane LightDirection
)
{
    v3_lane Result;

    v3_lane HalfDirection = Normalize(0.5f * (ViewDirection + LightDirection));

    v3_lane LightVector = Normalize
    (
        V3Lane
        (
            InnerProduct(LightDirection, SurfaceTangent),
            InnerProduct(LightDirection, SurfaceBiTangent),
            InnerProduct(LightDirection, SurfaceNormal)
        )
    );

    v3_lane HalfVector = Normalize
    (
        V3Lane
        (
            InnerProduct(HalfDirection, SurfaceTangent),
            InnerProduct(HalfDirection, SurfaceBiTangent),
            InnerProduct(HalfDirection, SurfaceNormal)
        )
    );

    coordinate_set_lane NewCoordinateSet;
    NewCoordinateSet.Z = HalfVector;
    NewCoordinateSet.Y = Normalize(CrossProduct(NewCoordinateSet.Z, SurfaceTangent));
    NewCoordinateSet.X = Normalize(CrossProduct(NewCoordinateSet.Y, NewCoordinateSet.Z));

#if 1
    // NOTE: most probably the opposite is true
    v3_lane NewLightVector = V3Lane
    (
        InnerProduct(NewCoordinateSet.X, LightVector),
        InnerProduct(NewCoordinateSet.Y, LightVector),
        InnerProduct(NewCoordinateSet.Z, LightVector)
    );
#else
    v3_lane NewLightVector = V3Lane
    (
        InnerProduct(LightVector, NewCoordinateSet.X),
        InnerProduct(LightVector, NewCoordinateSet.Y),
        InnerProduct(LightVector, NewCoordinateSet.Z)
    );
#endif

    for (u32 SubElementIndex = 0; SubElementIndex < SIMD_NUMBEROF_LANES; SubElementIndex++)
    {
        brdf_table *BrdfTable = &Materials[U32FromU32Lane(MaterialIndex, SubElementIndex)].BrdfTable;

        f32 HalfVectorTheta = Acos(V3FromV3Lane(HalfVector, SubElementIndex).Z);

        f32 NewLightVectorTheta = Acos(F32FromF32Lane(NewLightVector.Z, SubElementIndex));
        f32 NewLightVectorPhi = Atan2
        (
            F32FromF32Lane(NewLightVector.Y, SubElementIndex),
            F32FromF32Lane(NewLightVector.X, SubElementIndex)
        );

        if (NewLightVectorPhi < 0)
        {
            NewLightVectorPhi += PI32;
        }

        // ---------------------
        f32 F0 = SquareRoot(Clamp01(HalfVectorTheta / (0.5f * PI32)));
        u32 U0 = RoundF32ToU32((BrdfTable->SampleCount[0] - 1) * F0);

        f32 F1 = Clamp01(NewLightVectorTheta / (0.5f * PI32));
        u32 U1 = RoundF32ToU32((BrdfTable->SampleCount[1] - 1) * F1);

        f32 F2 = Clamp01(NewLightVectorPhi / PI32);
        u32 U2 = RoundF32ToU32((BrdfTable->SampleCount[2] - 1) * F2);

        u32 BrdfTableIndex =
            U2 +
            U1 * BrdfTable->SampleCount[2] +
            U0 * BrdfTable->SampleCount[2] * BrdfTable->SampleCount[1];
        Assert(BrdfTableIndex < BrdfTable->TotalSampleCount);
        // ---------------------

        v3 Color = BrdfTable->Samples[BrdfTableIndex];

        ((f32 *)&Result.X)[SubElementIndex] = Color.X;
        ((f32 *)&Result.Y)[SubElementIndex] = Color.Y;
        ((f32 *)&Result.Z)[SubElementIndex] = Color.Z;
    }

    return Result;
}