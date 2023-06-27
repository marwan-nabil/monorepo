#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include"platform.h"
#include "..\math\math.h"
#include"ray_tracer.h"

#include "..\math\scalars.cpp"
#include "..\math\vector2.cpp"
#include "..\math\vector3.cpp"
#include "..\math\vector4.cpp"

#define BOUNCES_PER_RAY 8
#define RAYS_PER_PIXEL 8

inline static void
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

inline static image_u32
CreateImage(u32 Width, u32 Height)
{
    image_u32 OutputImage = {};
    OutputImage.Width = Width;
    OutputImage.Height = Height;
    OutputImage.Pixels = (u32 *)malloc(OutputImage.Width * OutputImage.Height * sizeof(u32));
    return OutputImage;
}

inline static f32
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

inline static f32
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

static v3
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 Result = {};
    f32 HitDistanceLowerLimit = 0.001f;
    f32 ToleranceToZero = 0.0001f;
    v3 RayColorAttenuation = {1, 1, 1};

    for (u32 BounceIndex = 0; BounceIndex < BOUNCES_PER_RAY; BounceIndex++)
    {
        v3 NextRayNormal = {};

        f32 MinimumHitDistanceFound = F32MAX;
        b32 HitSomething = false;
        u32 HitMaterialIndex = 0;

        for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
        {
            plane *CurrentPlane = &World->Planes[PlaneIndex];
            f32 CurrentHitDistance = RayIntersectsPlane(RayOrigin, RayDirection, CurrentPlane, ToleranceToZero);
            if ((CurrentHitDistance < MinimumHitDistanceFound) && (CurrentHitDistance > HitDistanceLowerLimit))
            {
                HitSomething = true;
                MinimumHitDistanceFound = CurrentHitDistance;
                HitMaterialIndex = CurrentPlane->MaterialIndex;
                NextRayNormal = CurrentPlane->Normal;
            }
        }

        for (u32 SphereIndex = 0; SphereIndex < World->SpheresCount; SphereIndex++)
        {
            sphere *CurrentSphere = &World->Spheres[SphereIndex];
            f32 CurrentHitDistance = RayIntersectsSphere(RayOrigin, RayDirection, CurrentSphere, ToleranceToZero, HitDistanceLowerLimit);
            if ((CurrentHitDistance < MinimumHitDistanceFound) && (CurrentHitDistance > HitDistanceLowerLimit))
            {
                HitSomething = true;
                MinimumHitDistanceFound = CurrentHitDistance;
                HitMaterialIndex = CurrentSphere->MaterialIndex;
                NextRayNormal = Normalize(RayOrigin + CurrentHitDistance * RayDirection - CurrentSphere->Position);
            }
        }

        if (HitSomething)
        {
            material *HitMaterial = &World->Materials[HitMaterialIndex];
            
            Result += HadamardProduct(RayColorAttenuation, HitMaterial->EmmissionColor);

            f32 CosineAttenuationFactor = 1;
#if 0
            f32 CosineAttenuationFactor = InnerProduct(-RayDirection, NextRayNormal);
            if (CosineAttenuationFactor < 0)
            {
                CosineAttenuationFactor = 0;
            }
#endif
            RayColorAttenuation = HadamardProduct(RayColorAttenuation, CosineAttenuationFactor * HitMaterial->ReflectionColor);
            
            RayOrigin += MinimumHitDistanceFound * RayDirection;

            v3 PureBounceRayDirection = 
                Normalize(RayDirection - 2 * InnerProduct(RayDirection, NextRayNormal) * NextRayNormal);
            v3 RandomBounceRayDirection = 
                Normalize(NextRayNormal + V3(RandomBilateral(), RandomBilateral(), RandomBilateral()));
            RayDirection = Normalize(Lerp(RandomBounceRayDirection, PureBounceRayDirection, HitMaterial->Specularity));
        }
        else
        {
            material *HitMaterial = &World->Materials[HitMaterialIndex];
            Result += HadamardProduct(RayColorAttenuation, HitMaterial->EmmissionColor);
            break;
        }
    }

    return Result;
}

i32
main(i32 argc, u8 **argv)
{
    printf("--- RayCaster running ---\n");

    // image_u32 OutputImage = CreateImage(4096, 2048);
    image_u32 OutputImage = CreateImage(1280, 720);

    material MaterialsArray[5] = {};
    MaterialsArray[0].EmmissionColor = V3(0.3, 0.4, 0.5);
    MaterialsArray[1].ReflectionColor = V3(0.5, 0.5, 0.5);
    MaterialsArray[2].ReflectionColor = V3(0.7, 0.5, 0.3);
    MaterialsArray[3].ReflectionColor = V3(0.9, 0, 0);
    MaterialsArray[4].ReflectionColor = V3(0.2, 0.8, 0.2);
    MaterialsArray[4].Specularity = 0.7;

    plane PlanesArray[1] = {};
    PlanesArray[0].MaterialIndex = 1;
    PlanesArray[0].Normal = V3(0, 0, 1);
    PlanesArray[0].Distance = 0;

    sphere SpheresArray[3] = {};
    SpheresArray[0].MaterialIndex = 2;
    SpheresArray[0].Position = V3(0, 0, 0);
    SpheresArray[0].Radius = 1;

    SpheresArray[1].MaterialIndex = 3;
    SpheresArray[1].Position = V3(3, -2, 0);
    SpheresArray[1].Radius = 1;

    SpheresArray[2].MaterialIndex = 4;
    SpheresArray[2].Position = V3(-2, -1, 2);
    SpheresArray[2].Radius = 1;

    world World = {};
    World.Materials = MaterialsArray;
    World.MaterialsCount = ArrayLength(MaterialsArray);

    World.Planes = PlanesArray;
    World.PlanesCount = ArrayLength(PlanesArray);

    World.Spheres = SpheresArray;
    World.SpheresCount = ArrayLength(SpheresArray);

    coordinate_set WorldCoordinateSet = {};
    WorldCoordinateSet.X = V3(1, 0, 0);
    WorldCoordinateSet.Y = V3(0, 1, 0);
    WorldCoordinateSet.Z = V3(0, 0, 1);

    v3 CameraPosition = {0, -10, 1};

    coordinate_set CameraCoordinateSet = {};
    CameraCoordinateSet.Z = Normalize(CameraPosition);
    CameraCoordinateSet.X = Normalize(CrossProduct(CameraCoordinateSet.Z, WorldCoordinateSet.Z));
    CameraCoordinateSet.Y = Normalize(CrossProduct(CameraCoordinateSet.Z, CameraCoordinateSet.X));

    f32 FilmDistanceFromCamera = 1.0f;
    f32 FilmWidth = 1.0f;
    f32 FilmHeight = 1.0f;
    if (OutputImage.Width >= OutputImage.Height)
    {
        FilmHeight = (f32)OutputImage.Height / (f32)OutputImage.Width;
    }
    else
    {
        FilmWidth = (f32)OutputImage.Width / (f32)OutputImage.Height;
    }
    f32 HalfFilmWidth = 0.5f * FilmWidth;
    f32 HalfFilmHeight = 0.5f * FilmHeight;

    v3 FilmCenter = CameraPosition - FilmDistanceFromCamera * CameraCoordinateSet.Z;

    u32 *PixelWritePointer = OutputImage.Pixels;
    for (u32 Y = 0; Y < OutputImage.Height; Y++)
    {
        f32 FilmRatioY = 2.0f * ((f32)Y / (f32)OutputImage.Height) - 1.0f; // [-1, 1]
        for (u32 X = 0; X < OutputImage.Width; X++)
        {
            f32 FilmRatioX = 2.0f * ((f32)X / (f32)OutputImage.Width) - 1.0f; // [-1, 1]

            v3 PositionOnFilm = 
                FilmCenter +
                FilmRatioX * HalfFilmWidth * CameraCoordinateSet.X + 
                FilmRatioY * HalfFilmHeight * CameraCoordinateSet.Y; 

            v3 RayOrigin = CameraPosition;
            v3 RayDirection = Normalize(PositionOnFilm - CameraPosition);
            
            f32 SingleRayContributionRatio = 1.0 / (f32)RAYS_PER_PIXEL;
            v3 PixelColor = {};

            for (u32 RayIndex = 0; RayIndex < RAYS_PER_PIXEL; RayIndex++)
            {
                PixelColor +=  SingleRayContributionRatio * RayCast(&World, RayOrigin, RayDirection);
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

        if (Y % 64 == 0)
        {
            u32 PercentProgress = Y * 100 / OutputImage.Height;
            printf("\rRaycasting %d%%.", PercentProgress);
            fflush(stdout);
        }
    }

    WriteImage(OutputImage, "test.bmp");
    printf("\n--- Raycasting done ---\n");

    return 0;
}
