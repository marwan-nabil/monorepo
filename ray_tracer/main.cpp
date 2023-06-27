#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include"platform.h"
#include "..\math\math.h"
#include"ray_tracer.h"

#include "..\math\scalars.cpp"
#include "..\math\vector2.cpp"
#include "..\math\vector3.cpp"
#include "..\math\vector4.cpp"

#define BOUNCES_PER_RAY 8
#define RAYS_PER_PIXEL 8

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

inline f32
RayIntersectsPlane(v3 RayOrigin, v3 RayDirection, plane *Plane, f32 ToleranceToZero)
{
    f32 IntersectionDistance;

    f32 Denominator = InnerProduct(Plane->Normal, RayDirection);
    if (AbsoluteValue(Denominator) > ToleranceToZero)
    {
        f32 Numerator = -Plane->Distance - InnerProduct(Plane->Normal, RayOrigin);
        IntersectionDistance = Numerator / Denominator;
    }
    else
    {
        IntersectionDistance = F32MAX;
    }

    return IntersectionDistance;
}

inline f32
RayIntersectsSphere(v3 RayOrigin, v3 RayDirection, sphere *Sphere, f32 ToleranceToZero, f32 HitDistanceLowerLimit)
{
    f32 IntersectionDistance;

    v3 SphereRelativeRayOrigin = RayOrigin - Sphere->Position;

    f32 A = InnerProduct(RayDirection, RayDirection);
    f32 B = 2.0f * InnerProduct(RayDirection, SphereRelativeRayOrigin);
    f32 C = InnerProduct(SphereRelativeRayOrigin, SphereRelativeRayOrigin) - Square(Sphere->Radius);
    
    f32 RootTerm = SquareRoot(B * B - 4 * A * C);
    if (RootTerm > ToleranceToZero)
    {
        f32 QuadraticDenominator = 2 * A;
        f32 TPositive = (-B + RootTerm) / QuadraticDenominator;
        f32 TNegative = (-B - RootTerm) / QuadraticDenominator;

        if ((TNegative > HitDistanceLowerLimit) && (TNegative < TPositive)) 
        {
            IntersectionDistance = TNegative;
        }
        else
        {
            IntersectionDistance = TPositive;
        }
    }
    else
    {
        IntersectionDistance = F32MAX;
    }

    return IntersectionDistance;
}

inline u32 *
GetPixelPointer(image_u32 *Image, u32 X, u32 Y)
{
    u32 *Result = Image->Pixels + Y * Image->Width + X;
    return Result;
}

inline u32
LockedAddAndReturnOldValue(volatile u32 *Variable, u32 Addend)
{
}

inline u64
LockedAddAndReturnOldValue(volatile u64 *Variable, u64 Addend)
{
}

inline void
RenderTile(work_queue *WorkQueue)
{
    u32 WorkOrderIndex = LockedAddAndReturnOldValue(&WorkQueue->NextWorkOrderIndex, 1);
    if (WorkOrderIndex >= WorkQueue->WorkOrderCount)
    {
        return;
    }

    work_order *WorkOrder = &WorkQueue->WorkOrders[WorkOrderIndex];

    image_u32 *Image = WorkOrder->Image;
    world *World = WorkOrder->World;
    film *Film = &WorkOrder->World->Film;
    rendering_parameters *RenderingParameters = WorkOrder->RenderingParameters;

    f32 HalfFilmWidth = 0.5f * Film->Width;
    f32 HalfFilmHeight = 0.5f * Film->Height;
    f32 HalfPixelWidth = 0.5f * Film->PixelWidth;
    f32 HalfPixelHeight = 0.5f * Film->PixelHeight;
    v3 CameraX = World->Camera.CoordinateSet.X;
    v3 CameraY = World->Camera.CoordinateSet.Y;
    v3 CameraPosition = World->Camera.Position;

    f32 ToleranceToZero = RenderingParameters->ToleranceToZero;
    f32 HitDistanceLowerLimit = RenderingParameters->HitDistanceLowerLimit;

    u64 BouncesComputedPerTile = 0;

    for (u32 PixelY = WorkOrder->StartPixelY; PixelY < WorkOrder->EndPixelY; PixelY++)
    {
        f32 FilmRatioY = 2.0f * ((f32)PixelY / (f32)Image->Height) - 1.0f;

        u32 *PixelWritePointer = GetPixelPointer(Image, WorkOrder->StartPixelX, PixelY);

        for (u32 PixelX = WorkOrder->StartPixelX; PixelX < WorkOrder->EndPixelX; PixelX++)
        {
            f32 FilmRatioX = 2.0f * ((f32)PixelX / (f32)Image->Width) - 1.0f;

            v3 PixelColor = {};

            for (u32 RayIndex = 0; RayIndex < RAYS_PER_PIXEL; RayIndex++)
            {
                v3 RayColor = {};
                v3 RayColorAttenuation = {1, 1, 1};

                f32 FilmX = 
                    FilmRatioX * HalfFilmWidth + 
                    HalfPixelWidth * RandomBilateral();

                f32 FilmY = 
                    FilmRatioY * HalfFilmHeight + 
                    HalfPixelHeight * RandomBilateral();

                v3 RayPositionOnFilm = 
                    Film->Center +
                    FilmX * CameraX +
                    FilmY * CameraY;

                v3 BounceOrigin = CameraPosition;
                v3 BounceDirection = Normalize(RayPositionOnFilm - BounceOrigin);

                for (u32 BounceIndex = 0; BounceIndex < BOUNCES_PER_RAY; BounceIndex++)
                {
                    v3 NextBounceNormal = {};

                    BouncesComputedPerTile++;

                    f32 MinimumHitDistanceFound = F32MAX;
                    b32 HitSomething = false;
                    u32 HitMaterialIndex = 0;

                    for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
                    {
                        plane *CurrentPlane = &World->Planes[PlaneIndex];
                        f32 CurrentHitDistance = RayIntersectsPlane(BounceOrigin, BounceDirection, CurrentPlane, ToleranceToZero);
                        if 
                        (
                            (CurrentHitDistance < MinimumHitDistanceFound) && 
                            (CurrentHitDistance > HitDistanceLowerLimit)
                        )
                        {
                            HitSomething = true;
                            MinimumHitDistanceFound = CurrentHitDistance;
                            HitMaterialIndex = CurrentPlane->MaterialIndex;
                            NextBounceNormal = CurrentPlane->Normal;
                        }
                    }

                    for (u32 SphereIndex = 0; SphereIndex < World->SpheresCount; SphereIndex++)
                    {
                        sphere *CurrentSphere = &World->Spheres[SphereIndex];
                        f32 CurrentHitDistance = RayIntersectsSphere(BounceOrigin, BounceDirection, CurrentSphere, ToleranceToZero, HitDistanceLowerLimit);
                        if 
                        (
                            (CurrentHitDistance < MinimumHitDistanceFound) && 
                            (CurrentHitDistance > HitDistanceLowerLimit)
                        )
                        {
                            HitSomething = true;
                            MinimumHitDistanceFound = CurrentHitDistance;
                            HitMaterialIndex = CurrentSphere->MaterialIndex;
                            NextBounceNormal = Normalize(BounceOrigin + CurrentHitDistance * BounceDirection - CurrentSphere->Position);
                        }
                    }

                    if (HitSomething)
                    {
                        material *HitMaterial = &World->Materials[HitMaterialIndex];
                        
                        RayColor += HadamardProduct(RayColorAttenuation, HitMaterial->EmmissionColor);

                        f32 CosineAttenuationFactor = InnerProduct(-BounceDirection, NextBounceNormal);
                        if (CosineAttenuationFactor < 0)
                        {
                            CosineAttenuationFactor = 0;
                        }
                        RayColorAttenuation = HadamardProduct(RayColorAttenuation, CosineAttenuationFactor * HitMaterial->ReflectionColor);
                        
                        BounceOrigin += MinimumHitDistanceFound * BounceDirection;

                        v3 PureBounceDirection = 
                            Normalize(BounceDirection - 2 * InnerProduct(BounceDirection, NextBounceNormal) * NextBounceNormal);

                        v3 RandomBounceDirection = 
                            Normalize(NextBounceNormal + V3(RandomBilateral(), RandomBilateral(), RandomBilateral()));

                        BounceDirection = Normalize(Lerp(RandomBounceDirection, PureBounceDirection, HitMaterial->Specularity));
                    }
                    else
                    {
                        material *HitMaterial = &World->Materials[HitMaterialIndex];
                        RayColor += HadamardProduct(RayColorAttenuation, HitMaterial->EmmissionColor);
                        break;
                    }
                }

                PixelColor += RayColor / (f32)RAYS_PER_PIXEL;
            }

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
        World.Film.Width = 1.0f;
        World.Film.Height = (f32)OutputImage.Height / (f32)OutputImage.Width;
    }
    else
    {
        World.Film.Height = 1.0f;
        World.Film.Width = (f32)OutputImage.Width / (f32)OutputImage.Height;
    }

    World.Film.PixelWidth = 1.0f / (f32)OutputImage.Width;    
    World.Film.PixelHeight = 1.0f / (f32)OutputImage.Height;

    rendering_parameters RenderingParameters = {};
    RenderingParameters.CoreCount = 8;
    RenderingParameters.HitDistanceLowerLimit = 0.001f;
    RenderingParameters.ToleranceToZero = 0.0001f;

    RenderingParameters.TileWidthInPixels = OutputImage.Width / RenderingParameters.CoreCount;
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
        if (EndPixelY > OutputImage.Height)
        {
            EndPixelY = OutputImage.Height;
        }
        
        for (u32 TileX = 0; TileX < RenderingParameters.TileCountX; TileX++)
        {
            u32 StartPixelX = TileX * RenderingParameters.TileWidthInPixels;
            u32 EndPixelX = StartPixelX + RenderingParameters.TileWidthInPixels;
            if (EndPixelX > OutputImage.Width)
            {
                EndPixelX = OutputImage.Width;
            }

            work_order *WorkOrder = WorkQueue.WorkOrders + WorkQueue.WorkOrderCount++;
            Assert(WorkQueue.WorkOrderCount <= RenderingParameters.TotalTileCount);

            WorkOrder->World = &World;
            WorkOrder->Image = &OutputImage;
            WorkOrder->RenderingParameters = &RenderingParameters;
            WorkOrder->StartPixelX = StartPixelX;
            WorkOrder->StartPixelY = StartPixelY;
            WorkOrder->EndPixelX = EndPixelX;
            WorkOrder->EndPixelY = EndPixelY;
        }
    }

    Assert(WorkQueue.WorkOrderCount == RenderingParameters.TotalTileCount);

    for (u32 CoreIndex = 1; CoreIndex < RenderingParameters.CoreCount; CoreIndex++)
    {
        CreateThread();
    }

    clock_t StartTime = clock();
    
    while (WorkQueue.TotalTilesDone < RenderingParameters.TotalTileCount)
    {
        RenderTile(&WorkQueue);
        printf("\rRayCasting %d%%", 100 * WorkQueue.TotalTilesDone / RenderingParameters.TotalTileCount);
        fflush(stdout);
    }

    clock_t TotalTimeElapsed = clock() - StartTime;

    WriteImage(OutputImage, "test.bmp");

    printf("\nRayCasting time: %d ms\n", TotalTimeElapsed);
    printf("Core Count: %d\n", RenderingParameters.CoreCount);

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
