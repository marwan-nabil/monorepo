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
#include "..\math\random.h"
#include "..\math\vector2.h"
#include "..\math\vector3.h"
#include "..\math\vector4.h"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "..\simd_math\1_wide\math.h"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "..\simd_math\4_wide\math.h"
#   include "..\simd_math\4_wide\random.h"
#endif

#include"ray_tracer.h"

#include "..\math\random.cpp"
#include "..\math\integers.cpp"
#include "..\math\floats.cpp"
#include "..\math\vector2.cpp"
#include "..\math\vector3.cpp"
#include "..\math\vector4.cpp"

#if (SIMD_NUMBEROF_LANES == 1)
#   include "..\simd_math\1_wide\scalars.cpp"
#   include "..\simd_math\1_wide\v3.cpp"
#elif (SIMD_NUMBEROF_LANES == 4)
#   include "..\simd_math\4_wide\scalars.cpp"
#   include "..\simd_math\4_wide\v3.cpp"
#   include "..\simd_math\4_wide\random.cpp"
#   error "still not defined yet."
#endif

#include "..\multi_threading\utils.cpp"

inline void
WriteImage(image_u32 OutputImage, const char *FileName)
{
    u32 OutputPixelSize = OutputImage.Width * OutputImage.Height * sizeof(u32);

    bitmap_header BitmapHeader = {};
    BitmapHeader.FileType = 0x4D42;
    BitmapHeader.FileSize = sizeof(BitmapHeader) + OutputPixelSize;
    BitmapHeader.BitmapOffset = sizeof(BitmapHeader);
    BitmapHeader.Size = sizeof(BitmapHeader) - 14;
    BitmapHeader.Width = OutputImage.Width;
    BitmapHeader.Height = -(i32)OutputImage.Height;
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
    OutputImage.Width = Width;
    OutputImage.Height = Height;
    OutputImage.Pixels = (u32 *)malloc(OutputImage.Width * OutputImage.Height * sizeof(u32));
    return OutputImage;
}

inline f32_lane
RayIntersectsPlane(v3_lane RayOrigin, v3_lane RayDirection, plane *Plane, f32_lane ToleranceToZero)
{
    f32_lane IntersectionDistance = F32MAX;

    v3_lane PlaneNormal = Plane->Normal;
    f32_lane PlaneDistance = Plane->Distance;

    f32_lane Denominator = InnerProduct(PlaneNormal, RayDirection);
    u32_lane DenominatorMask = AbsoluteValue(Denominator) > ToleranceToZero;
    
    f32_lane Numerator;
    ConditionalAssign(&Numerator, -PlaneDistance - InnerProduct(PlaneNormal, RayOrigin), DenominatorMask);
    ConditionalAssign(&IntersectionDistance, Numerator / Denominator, DenominatorMask);

    return IntersectionDistance;
}

inline f32_lane
RayIntersectsSphere(v3_lane RayOrigin, v3_lane RayDirection, sphere *Sphere, f32_lane ToleranceToZero, f32_lane HitDistanceLowerLimit)
{
    f32_lane IntersectionDistance = F32MAX;

    v3_lane SpherePosition = Sphere->Position;
    f32_lane SphereRadius = Sphere->Radius;

    v3_lane SphereRelativeRayOrigin = RayOrigin - SpherePosition;

    f32_lane A = InnerProduct(RayDirection, RayDirection);
    f32_lane B = 2.0f * InnerProduct(RayDirection, SphereRelativeRayOrigin);
    f32_lane C = InnerProduct(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - Square(SphereRadius);
    
    f32_lane RootTerm = SquareRoot(B * B - 4 * A * C);
    u32_lane RootTermMask = RootTerm > ToleranceToZero;

    f32_lane QuadraticDenominator;
    ConditionalAssign(&QuadraticDenominator, 2 * A, RootTermMask);
    
    f32_lane TPositive;
    ConditionalAssign(&TPositive, (-B + RootTerm) / QuadraticDenominator, RootTermMask);

    f32_lane TNegative;
    ConditionalAssign(&TNegative, (-B - RootTerm) / QuadraticDenominator, RootTermMask);

    u32_lane TNegativeMask;
    ConditionalAssign(&TNegativeMask, (TNegative > HitDistanceLowerLimit) && (TNegative < TPositive), RootTermMask);

    u32_lane RootTermAndTNegativeMask = RootTermMask && TNegativeMask;

    ConditionalAssign(&IntersectionDistance, TPositive, RootTermMask);
    ConditionalAssign(&IntersectionDistance, TNegative, RootTermAndTNegativeMask);

    return IntersectionDistance;
}

inline u32 *
GetPixelPointer(image_u32 *Image, u32 X, u32 Y)
{
    u32 *Result = Image->Pixels + Y * Image->Width + X;
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

    v3_lane CameraX = World->Camera.CoordinateSet.X;
    v3_lane CameraY = World->Camera.CoordinateSet.Y;
    f32_lane HalfPixelWidth = World->Film.HalfPixelWidth;
    f32_lane HalfPixelHeight = World->Film.HalfPixelHeight;

    random_series *RandomSeries = &WorkOrder->Entropy;

    v3_lane CameraPosition = World->Camera.Position;
    f32_lane ToleranceToZero = WorkOrder->RenderingParameters->ToleranceToZero;
    f32_lane HitDistanceLowerLimit = WorkOrder->RenderingParameters->HitDistanceLowerLimit;

    for (u32 RayBatchIndex = 0; RayBatchIndex < RAY_BATCHES_PER_PIXEL; RayBatchIndex++)
    {
        v3_lane RayColor = {};
        v3_lane RayColorAttenuation = {1, 1, 1};

        v3_lane RayPositionOnFilm = 
            PixelCenterOnFilm +
            HalfPixelWidth * RandomBilateral(RandomSeries) * CameraX +
            HalfPixelHeight * RandomBilateral(RandomSeries) * CameraY;

        v3_lane BounceOrigin = CameraPosition;
        v3_lane BounceDirection = Normalize(RayPositionOnFilm - BounceOrigin);

        u32_lane BouncesComputedPerRayBatch = 0;
        u32_lane LaneMask = 0xFFFFFFFF;

        for (u32 BounceIndex = 0; BounceIndex < BOUNCES_PER_RAY; BounceIndex++)
        {
            v3_lane NextBounceNormal = {};

            u32_lane LaneIncrement = 1;
            BouncesComputedPerRayBatch += LaneIncrement & LaneMask;

            f32_lane MinimumHitDistanceFound = F32MAX;
            u32_lane HitMaterialIndex = 0;

            for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
            {
                plane *CurrentPlane = &World->Planes[PlaneIndex];
                f32_lane CurrentHitDistance = RayIntersectsPlane(BounceOrigin, BounceDirection, CurrentPlane, ToleranceToZero);

                v3_lane PlaneNormal = CurrentPlane->Normal;
                u32_lane PlaneMaterialIndex = CurrentPlane->MaterialIndex;

                u32_lane HitDistanceMask = 
                    (CurrentHitDistance < MinimumHitDistanceFound) && 
                    (CurrentHitDistance > HitDistanceLowerLimit);

                ConditionalAssign(&MinimumHitDistanceFound, CurrentHitDistance, HitDistanceMask);
                ConditionalAssign(&HitMaterialIndex, PlaneMaterialIndex, HitDistanceMask);
                ConditionalAssign(&NextBounceNormal, PlaneNormal, HitDistanceMask);
            }

            for (u32 SphereIndex = 0; SphereIndex < World->SpheresCount; SphereIndex++)
            {
                sphere *CurrentSphere = &World->Spheres[SphereIndex];

                v3_lane SpherePosition = CurrentSphere->Position;
                u32_lane SphereMaterialIndex = CurrentSphere->MaterialIndex;

                f32_lane CurrentHitDistance = RayIntersectsSphere(BounceOrigin, BounceDirection, CurrentSphere, ToleranceToZero, HitDistanceLowerLimit);

                u32_lane HitDistanceMask = 
                    (CurrentHitDistance < MinimumHitDistanceFound) && 
                    (CurrentHitDistance > HitDistanceLowerLimit);

                ConditionalAssign(&MinimumHitDistanceFound, CurrentHitDistance, HitDistanceMask);
                ConditionalAssign(&HitMaterialIndex, SphereMaterialIndex, HitDistanceMask);
                ConditionalAssign(&NextBounceNormal, Normalize(BounceOrigin + CurrentHitDistance * BounceDirection - SpherePosition), HitDistanceMask);
            }


            material *HitMaterial = &World->Materials[HitMaterialIndex];

            v3_lane MaterialEmmissionColor = LaneMask & HitMaterial->EmmissionColor; // must be masked for each lane
            v3_lane MaterialReflectionColor = HitMaterial->ReflectionColor;
            f32_lane MaterialSpecularity = HitMaterial->Specularity;

            RayColor += HadamardProduct(RayColorAttenuation, MaterialEmmissionColor);

            LaneMask = LaneMask & ((HitMaterialIndex != 0)? 0xFFFFFFFF : 0x00000000);

            f32_lane CosineAttenuationFactor = Max(InnerProduct(-BounceDirection, NextBounceNormal), 0);

            RayColorAttenuation = HadamardProduct(RayColorAttenuation, CosineAttenuationFactor * MaterialReflectionColor);
            
            BounceOrigin += MinimumHitDistanceFound * BounceDirection;

            v3_lane PureBounceDirection = 
                Normalize(BounceDirection - 2 * InnerProduct(BounceDirection, NextBounceNormal) * NextBounceNormal);

            v3_lane RandomBounceDirection = 
                Normalize(NextBounceNormal + V3(RandomBilateral(RandomSeries), RandomBilateral(RandomSeries), RandomBilateral(RandomSeries)));

            BounceDirection = Normalize(Lerp(RandomBounceDirection, PureBounceDirection, MaterialSpecularity));

            if (MaskIsAllZeroes(LaneMask))
            {
                break;
            }
        }

        PixelColor += HorizontalAdd(RayColor) / (f32)RAY_BATCHES_PER_PIXEL;
        BouncesComputedPerPixel += (u64)HorizontalAdd(BouncesComputedPerRayBatch);
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
        f32 FilmRatioY = 2.0f * ((f32)PixelY / (f32)Image->Height) - 1.0f;

        u32 *PixelWritePointer = GetPixelPointer(Image, WorkOrder->StartPixelX, PixelY);

        for (u32 PixelX = WorkOrder->StartPixelX; PixelX < WorkOrder->EndPixelX; PixelX++)
        {
            f32 FilmRatioX = 2.0f * ((f32)PixelX / (f32)Image->Width) - 1.0f;

            v3 PixelCenterOnFilm = 
                Film->Center +
                (FilmRatioX * Film->HalfWidth + Film->HalfPixelWidth) * CameraX +
                (FilmRatioY * Film->HalfHeight + Film->HalfPixelHeight) * CameraY;

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
WorkerThread(void *Parameter)
{
    work_queue *WorkQueue = (work_queue *)Parameter;
    while(RenderTile(WorkQueue)){}
    return 0;
}

i32
main(i32 argc, u8 **argv)
{
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

    if (OutputImage.Width >= OutputImage.Height)
    {
        World.Film.HalfWidth = 0.5f;
        World.Film.HalfHeight = 0.5f * (f32)OutputImage.Height / (f32)OutputImage.Width;
    }
    else
    {
        World.Film.HalfHeight = 1.0f;
        World.Film.HalfWidth = (f32)OutputImage.Width / (f32)OutputImage.Height;
    }

    World.Film.HalfPixelWidth = 0.5f / (f32)OutputImage.Width;
    World.Film.HalfPixelHeight = 0.5f / (f32)OutputImage.Height;

    rendering_parameters RenderingParameters = {};

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    RenderingParameters.CoreCount = (u8)SystemInfo.dwNumberOfProcessors;

    RenderingParameters.HitDistanceLowerLimit = 0.001f;
    RenderingParameters.ToleranceToZero = 0.0001f;

    // RenderingParameters.TileWidthInPixels = OutputImage.Width / RenderingParameters.CoreCount;
    RenderingParameters.TileWidthInPixels = 64; // TODO(marwan): optimize tile size
    RenderingParameters.TileHeightInPixels = RenderingParameters.TileWidthInPixels;

    RenderingParameters.TileCountX = 
        (OutputImage.Width + RenderingParameters.TileWidthInPixels - 1) / RenderingParameters.TileWidthInPixels;

    RenderingParameters.TileCountY = 
        (OutputImage.Height + RenderingParameters.TileHeightInPixels - 1) / RenderingParameters.TileHeightInPixels;

    RenderingParameters.TotalTileCount = RenderingParameters.TileCountX * RenderingParameters.TileCountY;

    work_queue WorkQueue = {};
    WorkQueue.WorkOrders = (work_order *)malloc(RenderingParameters.TotalTileCount * sizeof(work_order));

    for (u32 TileY = 0; TileY < RenderingParameters.TileCountY; TileY++)
    {
        u32 StartPixelY = TileY * RenderingParameters.TileHeightInPixels;
        u32 EndPixelY = StartPixelY + RenderingParameters.TileHeightInPixels;
        EndPixelY = Clamp(EndPixelY, 0, OutputImage.Height);
        
        for (u32 TileX = 0; TileX < RenderingParameters.TileCountX; TileX++)
        {
            u32 StartPixelX = TileX * RenderingParameters.TileWidthInPixels;
            u32 EndPixelX = StartPixelX + RenderingParameters.TileWidthInPixels;
            EndPixelX = Clamp(EndPixelX, 0, OutputImage.Width);

            work_order *WorkOrder = WorkQueue.WorkOrders + WorkQueue.WorkOrderCount++;
            WorkOrder->World = &World;
            WorkOrder->Image = &OutputImage;
            WorkOrder->RenderingParameters = &RenderingParameters;
            WorkOrder->StartPixelX = StartPixelX;
            WorkOrder->StartPixelY = StartPixelY;
            WorkOrder->EndPixelX = EndPixelX;
            WorkOrder->EndPixelY = EndPixelY;
            WorkOrder->Entropy.State = TileX * 32542345 + TileY * 897124 + 23523623;
        }
    }

    Assert(WorkQueue.WorkOrderCount == RenderingParameters.TotalTileCount);

    // TODO(marwan): memory fence here
    clock_t StartTime = clock();

    for (u32 CoreIndex = 1; CoreIndex < RenderingParameters.CoreCount; CoreIndex++)
    {
        DWORD ThreadId;
        HANDLE ThreadHandle = CreateThread(0, 0, WorkerThread, &WorkQueue, 0, &ThreadId);
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

    WriteImage(OutputImage, "test.bmp");

    printf("\nRayCasting time: %d ms\n", TotalTimeElapsed);
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

    return 0;
}
