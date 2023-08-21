#include <Windows.h>
#include <intrin.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include"platform.h"

#include "..\math\constants.h"
#include "..\math\macros.h"
#include "..\math\random.h"
#include "..\math\vector2.h"
#include "..\math\vector3.h"
#include "..\math\vector4.h"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "..\simd_math\1_wide\math.h"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "..\simd_math\4_wide\math.h"
#else
#   error "the defined SIMD_NUMBEROF_LANES is still not supported"
#endif

#if (SIMD_NUMBEROF_LANES != 1)
#   include "..\simd_math\shared\random.h"
#   include "..\simd_math\shared\math.h"
#endif

#include "ray_tracer.h"

#include "..\multi_threading\utils.cpp"

#include "..\math\random.cpp"
#include "..\math\conversions.cpp"
#include "..\math\integers.cpp"
#include "..\math\floats.cpp"
#include "..\math\vector2.cpp"
#include "..\math\vector3.cpp"
#include "..\math\vector4.cpp"
#include "..\math\assertions.cpp"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "..\simd_math\1_wide\conversions.cpp"
#   include "..\simd_math\1_wide\scalars.cpp"
#   include "..\simd_math\1_wide\vector3.cpp"
#   include "..\simd_math\1_wide\random.cpp"
#   include "..\simd_math\1_wide\assertions.cpp"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "..\simd_math\4_wide\conversions.cpp"
#   include "..\simd_math\4_wide\assertions.cpp"
#   include "..\simd_math\4_wide\integers.cpp"
#   include "..\simd_math\4_wide\floats.cpp"
#else
#   error "the defined SIMD_NUMBEROF_LANES is still not supported"
#endif

#if (SIMD_NUMBEROF_LANES != 1)
#   include "..\simd_math\shared\conversions.cpp"
#   include "..\simd_math\shared\integers.cpp"
#   include "..\simd_math\shared\floats.cpp"
#   include "..\simd_math\shared\vector3.cpp"
#   include "..\simd_math\shared\random.cpp"
#endif

inline void
WriteImage(image_u32 OutputImage, const char *FileName)
{
    u32 OutputPixelSize = OutputImage.WidthInPixels * OutputImage.HeightInPixels * sizeof(u32);

    bitmap_header BitmapHeader = {};
    BitmapHeader.FileType = 0x4D42;
    BitmapHeader.FileSize = sizeof(BitmapHeader) + OutputPixelSize;
    BitmapHeader.BitmapOffset = sizeof(BitmapHeader);
    BitmapHeader.Size = sizeof(BitmapHeader) - 14;
    BitmapHeader.Width = OutputImage.WidthInPixels;
    BitmapHeader.Height = -(i32)OutputImage.HeightInPixels;
    BitmapHeader.Planes = 1;
    BitmapHeader.BitsPerPixel = 32;
    BitmapHeader.Compression = 0;
    BitmapHeader.SizeOfBitmap = OutputPixelSize;
    BitmapHeader.HorizontalResolution = 0;
    BitmapHeader.VerticalResolution = 0;
    BitmapHeader.ColorsUsed = 0;
    BitmapHeader.ColorsImportant = 0;

    FILE *OutputFile = fopen(FileName, "wb");
    if (OutputFile)
    {
        fwrite(&BitmapHeader, sizeof(BitmapHeader), 1, OutputFile);
        fwrite((void *)OutputImage.Pixels, OutputPixelSize, 1, OutputFile);
        fclose(OutputFile);
    }
    else
    {
        printf("ERROR: unable to create the output bitmap file.\n");
    }
}

inline image_u32
CreateImage(u32 Width, u32 Height)
{
    image_u32 OutputImage = {};
    OutputImage.WidthInPixels = Width;
    OutputImage.HeightInPixels = Height;
    OutputImage.Pixels = (u32 *)malloc
    (
        OutputImage.WidthInPixels * 
        OutputImage.HeightInPixels * 
        sizeof(u32)
    );
    return OutputImage;
}

inline u32 *
GetPixelPointer(image_u32 *Image, u32 X, u32 Y)
{
    u32 *Result = Image->Pixels + Y * Image->WidthInPixels + X;
    return Result;
}

inline v3
RenderPixel
(
    work_order *WorkOrder, v3 PixelCenterOnFilm, u64 *BouncesComputedPerTile
)
{
    v3 PixelColor = {};
    u64 BouncesComputedPerPixel = 0;

    world *World = WorkOrder->World;
    v3_lane CameraX = V3LaneFromV3(World->Camera.CoordinateSet.X);
    v3_lane CameraY = V3LaneFromV3(World->Camera.CoordinateSet.Y);
    f32_lane HalfPixelWidth = F32LaneFromF32(World->Film.HalfPixelWidth);
    f32_lane HalfPixelHeight = F32LaneFromF32(World->Film.HalfPixelHeight);
    random_series_lane *RandomSeries = &WorkOrder->Entropy;
    v3_lane CameraPosition = V3LaneFromV3(World->Camera.Position);
    f32_lane ToleranceToZero = F32LaneFromF32(WorkOrder->RenderingParameters->ToleranceToZero);
    f32_lane HitDistanceLowerLimit = F32LaneFromF32(WorkOrder->RenderingParameters->HitDistanceLowerLimit);

    for (u32 RayBatchIndex = 0; RayBatchIndex < RAY_BATCHES_PER_PIXEL; RayBatchIndex++)
    {
        v3_lane RayBatchColor = V3Lane(0, 0, 0);
        v3_lane RayBatchColorAttenuation = V3Lane(1, 1, 1);

        v3_lane RayBatchPositionOnFilm = 
            V3LaneFromV3(PixelCenterOnFilm) +
            HalfPixelWidth * RandomBilateralLane(RandomSeries) * CameraX +
            HalfPixelHeight * RandomBilateralLane(RandomSeries) * CameraY;

        v3_lane BounceOrigin = CameraPosition;
        v3_lane BounceDirection = Normalize(RayBatchPositionOnFilm - BounceOrigin);

        u32_lane BouncesComputedPerRayBatch = U32LaneFromU32(0);
        u32_lane LaneMask = U32LaneFromU32(0xFFFFFFFF);

        for (u32 BounceIndex = 0; BounceIndex < BOUNCES_PER_RAY; BounceIndex++)
        {
            v3_lane NextBounceNormal = {};

            BouncesComputedPerRayBatch += U32LaneFromU32(1) & LaneMask;

            f32_lane MinimumHitDistanceFound = F32LaneFromF32(F32MAX);
            u32_lane HitMaterialIndex = U32LaneFromU32(0);

            for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
            {
                plane *CurrentPlane = &World->Planes[PlaneIndex];
                v3_lane PlaneNormal = V3LaneFromV3(CurrentPlane->Normal);
                f32_lane PlaneDistance = F32LaneFromF32(CurrentPlane->Distance);

                f32_lane Denominator = InnerProduct(PlaneNormal, BounceDirection);
                u32_lane DenominatorMask = (Denominator > ToleranceToZero) | (Denominator < -ToleranceToZero);

                if (!MaskIsAllZeroes(DenominatorMask))
                {
                    f32_lane CurrentHitDistance = (-PlaneDistance - InnerProduct(PlaneNormal, BounceOrigin)) / Denominator;
                    u32_lane CurrentHitDistanceMask = 
                        (CurrentHitDistance < MinimumHitDistanceFound) &
                        (CurrentHitDistance > HitDistanceLowerLimit);

                    u32_lane HitMask = CurrentHitDistanceMask & DenominatorMask;
                    if (!MaskIsAllZeroes(HitMask))
                    {
                        ConditionalAssign(&MinimumHitDistanceFound, CurrentHitDistance, HitMask);
                        ConditionalAssign(&HitMaterialIndex, U32LaneFromU32(CurrentPlane->MaterialIndex), HitMask);
                        ConditionalAssign(&NextBounceNormal, PlaneNormal, HitMask);
                    }
                }
            }

            for (u32 SphereIndex = 0; SphereIndex < World->SpheresCount; SphereIndex++)
            {
                sphere *CurrentSphere = &World->Spheres[SphereIndex];
                v3_lane SpherePosition = V3LaneFromV3(CurrentSphere->Position);
                f32_lane SphereRadius = F32LaneFromF32(CurrentSphere->Radius);
                u32_lane SphereMaterialIndex = U32LaneFromU32(CurrentSphere->MaterialIndex);
                v3_lane SphereRelativeRayOrigin = BounceOrigin - SpherePosition;

                f32_lane A = InnerProduct(BounceDirection, BounceDirection);
                f32_lane B = 2.0f * InnerProduct(BounceDirection, SphereRelativeRayOrigin);
                f32_lane C = InnerProduct(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - Square(SphereRadius);

                f32_lane RootTerm = SquareRoot(B * B - 4 * A * C);
                u32_lane RootTermMask = RootTerm > ToleranceToZero;

                f32_lane QuadraticDenominator = 2 * A;

                if (!MaskIsAllZeroes(RootTermMask))
                {
                    f32_lane PositiveSolution = (-B + RootTerm) / QuadraticDenominator;
                    f32_lane NegativeSolution = (-B - RootTerm) / QuadraticDenominator;

                    u32_lane NegativeSolutionMask = (NegativeSolution > HitDistanceLowerLimit) & (NegativeSolution < PositiveSolution);

                    f32_lane CurrentHitDistance = PositiveSolution;
                    ConditionalAssign(&CurrentHitDistance, NegativeSolution, NegativeSolutionMask);

                    u32_lane HitDistanceMask = 
                        (CurrentHitDistance < MinimumHitDistanceFound) &
                        (CurrentHitDistance > HitDistanceLowerLimit);

                    if (!MaskIsAllZeroes(HitDistanceMask))
                    {
                        u32_lane HitMask = RootTermMask & HitDistanceMask;

                        ConditionalAssign(&MinimumHitDistanceFound, CurrentHitDistance, HitMask);
                        ConditionalAssign(&HitMaterialIndex, SphereMaterialIndex, HitMask);
                        ConditionalAssign(&NextBounceNormal, Normalize(BounceOrigin + MinimumHitDistanceFound * BounceDirection - SpherePosition), HitDistanceMask);
                    }
                }
            }

#if (SIMD_NUMBEROF_LANES == 1)
            material *HitMaterial = &World->Materials[HitMaterialIndex];
            f32_lane HitMaterialSpecularity = HitMaterial->Specularity;
            v3_lane HitMaterialReflectionColor = HitMaterial->ReflectionColor;
            v3_lane HitMaterialEmmissionColor = HitMaterial->EmmissionColor;
#elif (SIMD_NUMBEROF_LANES == 4)
            f32_lane HitMaterialSpecularity = 
                LaneMask & GatherF32(World->Materials, Specularity, HitMaterialIndex);

            v3_lane HitMaterialReflectionColor = 
                LaneMask & GatherV3(World->Materials, ReflectionColor, HitMaterialIndex);

            v3_lane HitMaterialEmmissionColor = 
                LaneMask & GatherV3(World->Materials, EmmissionColor, HitMaterialIndex);
#endif

            RayBatchColor += HadamardProduct(RayBatchColorAttenuation, HitMaterialEmmissionColor);

            LaneMask = LaneMask & (HitMaterialIndex != U32LaneFromU32(0));

            f32_lane CosineAttenuationFactor = Max(InnerProduct(-BounceDirection, NextBounceNormal), F32LaneFromF32(0));

            RayBatchColorAttenuation = HadamardProduct(RayBatchColorAttenuation, CosineAttenuationFactor * HitMaterialReflectionColor);

            BounceOrigin += MinimumHitDistanceFound * BounceDirection;

            v3_lane PureBounceDirection = 
                Normalize(BounceDirection - 2 * InnerProduct(BounceDirection, NextBounceNormal) * NextBounceNormal);

            v3_lane RandomBounceDirection = 
                Normalize(NextBounceNormal + V3Lane(RandomBilateralLane(RandomSeries), RandomBilateralLane(RandomSeries), RandomBilateralLane(RandomSeries)));

            BounceDirection = Normalize(Lerp(RandomBounceDirection, PureBounceDirection, HitMaterialSpecularity));

            if (MaskIsAllZeroes(LaneMask))
            {
                break;
            }
        }

        PixelColor += HorizontalAdd(RayBatchColor) / (f32)RAYS_PER_PIXEL;
        BouncesComputedPerPixel += HorizontalAdd(BouncesComputedPerRayBatch);
    }

    *BouncesComputedPerTile += BouncesComputedPerPixel;
    WorkOrder->Entropy = *RandomSeries;
    return PixelColor;
}

inline b32
RenderTile(work_queue *WorkQueue)
{
    u64 WorkOrderIndex = LockedAddAndReturnOldValue(&WorkQueue->NextWorkOrderIndex, 1);
    if (WorkOrderIndex >= WorkQueue->WorkOrderCount)
    {
        return false;
    }

    work_order *WorkOrder = &WorkQueue->WorkOrders[WorkOrderIndex];

    image_u32 *Image = WorkOrder->Image;
    world *World = WorkOrder->World;
    film *Film = &WorkOrder->World->Film;
    rendering_parameters *RenderingParameters = WorkOrder->RenderingParameters;
    v3 CameraX = World->Camera.CoordinateSet.X;
    v3 CameraY = World->Camera.CoordinateSet.Y;

    u64 BouncesComputedPerTile = 0;

    for (u32 PixelY = WorkOrder->StartPixelY; PixelY < WorkOrder->EndPixelY; PixelY++)
    {
        f32 PixelBeginY = 2.0f * ((f32)PixelY / (f32)Image->HeightInPixels) - 1.0f;

        u32 *PixelWritePointer = GetPixelPointer(Image, WorkOrder->StartPixelX, PixelY);

        for (u32 PixelX = WorkOrder->StartPixelX; PixelX < WorkOrder->EndPixelX; PixelX++)
        {
            f32 PixelBeginX = 2.0f * ((f32)PixelX / (f32)Image->WidthInPixels) - 1.0f;

            v3 PixelCenterOnFilm = 
                Film->Center +
                (PixelBeginX * Film->HalfWidth + Film->HalfPixelWidth) * CameraX +
                (PixelBeginY * Film->HalfHeight + Film->HalfPixelHeight) * CameraY;

            v3 PixelColor = RenderPixel
            (
                WorkOrder, PixelCenterOnFilm, &BouncesComputedPerTile
            );

            v4 BitmapColorRGBA = 
            {
                255 * TranslateLinearTosRGB(PixelColor.Red),
                255 * TranslateLinearTosRGB(PixelColor.Green),
                255 * TranslateLinearTosRGB(PixelColor.Blue),
                255
            };

            *PixelWritePointer++ =
            (
                (RoundF32ToU32(BitmapColorRGBA.Alpha) << 24) |
                (RoundF32ToU32(BitmapColorRGBA.Red) << 16) |
                (RoundF32ToU32(BitmapColorRGBA.Green) << 8) |
                (RoundF32ToU32(BitmapColorRGBA.Blue) << 0)
            );
        }
    }

    LockedAddAndReturnOldValue(&WorkQueue->TotalTilesDone, 1);
    LockedAddAndReturnOldValue(&WorkQueue->TotalRayBouncesComputed, BouncesComputedPerTile);
    return true;
}

static DWORD WINAPI
WorkerThreadEntry(void *Parameter)
{
    work_queue *WorkQueue = (work_queue *)Parameter;
    while (RenderTile(WorkQueue)){}
    return 0;
}

i32
main(i32 argc, u8 **argv)
{
#if 0
    printf("Testing...\n");

    v3_lane Vector0 = V3LaneFromV3
    (
        V3(5000, 5000, 5000),
        V3(5000, 5000, 5000),
        V3(5000, 5000, 5000),
        V3(5000, 5000, 5000)
    );

    u32_lane Mask = U32LaneFromU32(0xFFFFFFFF, 0xFFFFFFFF, 0, 0);

    v3_lane Vector1 = Mask & Vector0;

    printf("Vecor1.X[0] = %f\n", F32FromF32Lane(Vector1.X, 0));
    printf("Vecor1.X[1] = %f\n", F32FromF32Lane(Vector1.X, 1));
    printf("Vecor1.X[2] = %f\n", F32FromF32Lane(Vector1.X, 2));
    printf("Vecor1.X[3] = %f\n", F32FromF32Lane(Vector1.X, 3));

    printf("Vecor1.Y[0] = %f\n", F32FromF32Lane(Vector1.Y, 0));
    printf("Vecor1.Y[1] = %f\n", F32FromF32Lane(Vector1.Y, 1));
    printf("Vecor1.Y[2] = %f\n", F32FromF32Lane(Vector1.Y, 2));
    printf("Vecor1.Y[3] = %f\n", F32FromF32Lane(Vector1.Y, 3));

    printf("Vecor1.Z[0] = %f\n", F32FromF32Lane(Vector1.Z, 0));
    printf("Vecor1.Z[1] = %f\n", F32FromF32Lane(Vector1.Z, 1));
    printf("Vecor1.Z[2] = %f\n", F32FromF32Lane(Vector1.Z, 2));
    printf("Vecor1.Z[3] = %f\n", F32FromF32Lane(Vector1.Z, 3));

#else
    printf("RayCasting...");

    image_u32 OutputImage = CreateImage(1280, 720);

    material MaterialsArray[7] = {};
    MaterialsArray[0].EmmissionColor = V3(0.3, 0.4, 0.5);
    MaterialsArray[1].ReflectionColor = V3(0.5, 0.5, 0.5);
    MaterialsArray[2].ReflectionColor = V3(0.7, 0.5, 0.3);
    MaterialsArray[3].EmmissionColor = V3(4, 0, 0);
    MaterialsArray[4].ReflectionColor = V3(0.2, 0.8, 0.2);
    MaterialsArray[4].Specularity = 0.7;
    MaterialsArray[5].ReflectionColor = V3(0.4, 0.8, 0.9);
    MaterialsArray[5].Specularity = 0.85;
    MaterialsArray[6].ReflectionColor = V3(0.95, 0.95, 0.95);
    MaterialsArray[6].Specularity = 1;

    plane PlanesArray[1] = {};
    PlanesArray[0].MaterialIndex = 1;
    PlanesArray[0].Normal = V3(0, 0, 1);
    PlanesArray[0].Distance = 0;

    sphere SpheresArray[5] = {};
    SpheresArray[0].MaterialIndex = 2;
    SpheresArray[0].Position = V3(0, 0, 0);
    SpheresArray[0].Radius = 1;
    SpheresArray[1].MaterialIndex = 3;
    SpheresArray[1].Position = V3(3, -2, 0);
    SpheresArray[1].Radius = 1;
    SpheresArray[2].MaterialIndex = 4;
    SpheresArray[2].Position = V3(-2, -1, 2);
    SpheresArray[2].Radius = 1;
    SpheresArray[3].MaterialIndex = 5;
    SpheresArray[3].Position = V3(1, -1, 3);
    SpheresArray[3].Radius = 1;
    SpheresArray[4].MaterialIndex = 6;
    SpheresArray[4].Position = V3(-2, 3, 0);
    SpheresArray[4].Radius = 2;

    world World = {};
    World.Materials = MaterialsArray;
    World.MaterialsCount = ArrayLength(MaterialsArray);

    World.Planes = PlanesArray;
    World.PlanesCount = ArrayLength(PlanesArray);

    World.Spheres = SpheresArray;
    World.SpheresCount = ArrayLength(SpheresArray);

    World.CoordinateSet.X = V3(1, 0, 0);
    World.CoordinateSet.Y = V3(0, 1, 0);
    World.CoordinateSet.Z = V3(0, 0, 1);

    World.Camera.Position = {0, -10, 1};
    World.Camera.CoordinateSet.Z = Normalize(World.Camera.Position);
    World.Camera.CoordinateSet.X = Normalize(CrossProduct(World.Camera.CoordinateSet.Z, World.CoordinateSet.Z));
    World.Camera.CoordinateSet.Y = Normalize(CrossProduct(World.Camera.CoordinateSet.Z, World.Camera.CoordinateSet.X));

    World.Film.DistanceFromCamera = 1.0f;
    World.Film.Center = World.Camera.Position + World.Film.DistanceFromCamera * (-World.Camera.CoordinateSet.Z);

    if (OutputImage.WidthInPixels >= OutputImage.HeightInPixels)
    {
        World.Film.HalfWidth = 0.5f;
        World.Film.HalfHeight = 0.5f * (f32)OutputImage.HeightInPixels / (f32)OutputImage.WidthInPixels;
    }
    else
    {
        World.Film.HalfHeight = 1.0f;
        World.Film.HalfWidth = (f32)OutputImage.WidthInPixels / (f32)OutputImage.HeightInPixels;
    }

    World.Film.HalfPixelWidth = 0.5f / (f32)OutputImage.WidthInPixels;
    World.Film.HalfPixelHeight = 0.5f / (f32)OutputImage.HeightInPixels;

    rendering_parameters RenderingParameters = {};

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    RenderingParameters.CoreCount = (u8)SystemInfo.dwNumberOfProcessors;
    // RenderingParameters.CoreCount = 1;

    RenderingParameters.HitDistanceLowerLimit = 0.001f;
    RenderingParameters.ToleranceToZero = 0.0001f;

    RenderingParameters.TileWidthInPixels = OutputImage.WidthInPixels / RenderingParameters.CoreCount;
    // RenderingParameters.TileWidthInPixels = 64; // TODO: optimize tile size
    RenderingParameters.TileHeightInPixels = RenderingParameters.TileWidthInPixels;

    RenderingParameters.TileCountX = 
        (OutputImage.WidthInPixels + RenderingParameters.TileWidthInPixels - 1) / RenderingParameters.TileWidthInPixels;

    RenderingParameters.TileCountY = 
        (OutputImage.HeightInPixels + RenderingParameters.TileHeightInPixels - 1) / RenderingParameters.TileHeightInPixels;

    RenderingParameters.TotalTileCount = RenderingParameters.TileCountX * RenderingParameters.TileCountY;

    work_queue WorkQueue = {};
    WorkQueue.WorkOrders = (work_order *)malloc(RenderingParameters.TotalTileCount * sizeof(work_order));

    for (u32 TileY = 0; TileY < RenderingParameters.TileCountY; TileY++)
    {
        u32 StartPixelY = TileY * RenderingParameters.TileHeightInPixels;
        u32 EndPixelY = StartPixelY + RenderingParameters.TileHeightInPixels;
        EndPixelY = Clamp(EndPixelY, 0, OutputImage.HeightInPixels);
        
        for (u32 TileX = 0; TileX < RenderingParameters.TileCountX; TileX++)
        {
            u32 StartPixelX = TileX * RenderingParameters.TileWidthInPixels;
            u32 EndPixelX = StartPixelX + RenderingParameters.TileWidthInPixels;
            EndPixelX = Clamp(EndPixelX, 0, OutputImage.WidthInPixels);

            work_order *WorkOrder = WorkQueue.WorkOrders + WorkQueue.WorkOrderCount++;
            WorkOrder->World = &World;
            WorkOrder->Image = &OutputImage;
            WorkOrder->RenderingParameters = &RenderingParameters;
            WorkOrder->StartPixelX = StartPixelX;
            WorkOrder->StartPixelY = StartPixelY;
            WorkOrder->EndPixelX = EndPixelX;
            WorkOrder->EndPixelY = EndPixelY;

#if (SIMD_NUMBEROF_LANES != 1)
            WorkOrder->Entropy.State = U32LaneFromU32
            (
                TileX * 32542345 + TileY * 881712265 + 93073411,
                TileX * 98698641 + TileY * 640200962 + 24681141,
                TileX * 52350329 + TileY * 793083851 + 63274279,
                TileX * 39846279 + TileY * 505147656 + 12932640
            );
#else
            WorkOrder->Entropy.State = TileX * 52350329 + TileY * 793083851 + 63274279;
#endif
        }
    }

    // TODO: memory fence here

    clock_t StartTime = clock();

    for (u32 CoreIndex = 1; CoreIndex < RenderingParameters.CoreCount; CoreIndex++)
    {
        DWORD ThreadId;
        HANDLE ThreadHandle = CreateThread(0, 0, WorkerThreadEntry, &WorkQueue, 0, &ThreadId);
        CloseHandle(ThreadHandle);
    }
    
    while (WorkQueue.TotalTilesDone < RenderingParameters.TotalTileCount)
    {
        if (RenderTile(&WorkQueue))
        {
            printf("\rRayCasting %d%%", 100 * (u32)WorkQueue.TotalTilesDone / RenderingParameters.TotalTileCount);
            fflush(stdout);
        }
    }

    clock_t TotalTimeElapsed = clock() - StartTime;

    WriteImage(OutputImage, (const char *)argv[1]);

    printf("\nRayCasting time: %ld ms\n", TotalTimeElapsed);
    printf("Core Count: %d\n", RenderingParameters.CoreCount);
    printf("Rays Per Pixel: %d\n", RAYS_PER_PIXEL);
    printf("Bounces Per Ray: %d\n", BOUNCES_PER_RAY);

    f32 KBytesPerTile = (f32)
    (
        RenderingParameters.TileWidthInPixels * 
        RenderingParameters.TileHeightInPixels * 
        sizeof(u32) / 
        1024.0f
    );

    printf
    (
        "Using %d %dx%d tiles, %.2f KBytes/tile\n", 
        RenderingParameters.TotalTileCount, 
        RenderingParameters.TileWidthInPixels, RenderingParameters.TileHeightInPixels, 
        KBytesPerTile
    );
    printf("Bounces Computed: %lld\n", WorkQueue.TotalRayBouncesComputed);
    printf
    (
        "performance metric: %f ns/bounce\n", 
        (f64)TotalTimeElapsed / (f64)WorkQueue.TotalRayBouncesComputed * 1000000.0f
    );

#endif
    return 0;
}
