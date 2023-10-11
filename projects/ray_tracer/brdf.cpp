static void
LoadMerlBrdfFile(char *FileName, brdf_table *ResultTable)
{
    FILE *File = fopen(FileName, "rb");
    if (!File)
    {
        fprintf(stderr, "unable to open MERL BRDF file: %s\n", FileName);
        return;
    }

    fread(ResultTable->SampleCount, sizeof(ResultTable->SampleCount), 1, File);
    u32 TotalSampleCount =
        ResultTable->SampleCount[0] * 
        ResultTable->SampleCount[1] * 
        ResultTable->SampleCount[2];

    u32 TotalReadSize = TotalSampleCount * sizeof(f64);
    u32 ResultTableSize = TotalSampleCount * sizeof(f32);

    f64 *TemporarySamples = (f64 *)malloc(TotalReadSize);
    ResultTable->Samples = (f32 *)malloc(ResultTableSize);

    fread(TemporarySamples, TotalReadSize, 1, File);

    for (u32 SampleIndex = 0; SampleIndex < TotalSampleCount; SampleIndex++)
    {
        ResultTable->Samples[SampleIndex] = (f32)TemporarySamples[SampleIndex];
    }

    fclose(File);
    free(TemporarySamples);
}