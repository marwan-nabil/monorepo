#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include"platform.h"
#include "..\math\math.h"
#include"ray_tracer.h"

#include "..\math\math.cpp"

static void
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

static image_u32
CreateImage(u32 Width, u32 Height)
{
    image_u32 OutputImage = {};
    OutputImage.Width = 1280;
    OutputImage.Height = 720;
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

static v3
RayCast(world *World, v3 RayOrigin, v3 RayDirection)
{
    v3 Result = World->Materials[0].Color;

    f32 MinimumHitDistance = F32MAX;

    for (u32 PlaneIndex = 0; PlaneIndex < World->PlanesCount; PlaneIndex++)
    {
        plane *CurrentPlane = &World->Planes[PlaneIndex];
        f32 CurrentHitDistance = RayIntersectsPlane(RayOrigin, RayDirection, CurrentPlane, 0.0001f);
        if
        ((CurrentHitDistance < MinimumHitDistance) && (CurrentHitDistance > 0))
        {
            MinimumHitDistance = CurrentHitDistance;
            Result = World->Materials[CurrentPlane->MaterialIndex].Color;
        }
    }

    return Result;
}

i32
main(i32 argc, u8 **argv)
{
    printf("--- raytracer running ---\n");
    image_u32 OutputImage = CreateImage(1280, 720);

    material MaterialsArray[3] = {};
    MaterialsArray[0].Color = V3(0.2, 0.2, 0.2); // dark grey
    MaterialsArray[1].Color = V3(1, 0, 0); // red
    MaterialsArray[2].Color = V3(0, 1, 0); // green

    plane PlanesArray[1] = {};
    PlanesArray[0].MaterialIndex = 1;
    PlanesArray[0].Normal = V3(0, 0, 1);
    PlanesArray[0].Distance = 0;
    // PlanesArray[1].MaterialIndex = 2;
    // PlanesArray[1].Normal = V3(0, 1, 1);
    // PlanesArray[1].Distance = -3;
   
    sphere SpheresArray[1] = {};
    SpheresArray[0].MaterialIndex = 2;
    SpheresArray[0].Position = V3(0, 0, 0);
    SpheresArray[0].Radius = 2;

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

    v3 CameraPosition = {0, 10, 1};

    coordinate_set CameraCoordinateSet = {};
    CameraCoordinateSet.Z = Normalize(CameraPosition);
    CameraCoordinateSet.X = Normalize(CrossProduct(CameraCoordinateSet.Z, WorldCoordinateSet.Z));
    CameraCoordinateSet.Y = Normalize(CrossProduct(CameraCoordinateSet.Z, CameraCoordinateSet.X));

    f32 FilmDistanceFromCamera = 1.0f;
    f32 FilmWidth = 1.0f;
    f32 HalfFilmWidth = 0.5f * FilmWidth;
    f32 FilmHeight = 1.0f;
    f32 HalfFilmHeight = 0.5f * FilmHeight;

    v3 FilmCenter = CameraPosition - FilmDistanceFromCamera * CameraCoordinateSet.Z;

    u32 *PixelWrite = OutputImage.Pixels;
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
            
            v3 PixelColor = RayCast(&World, RayOrigin, RayDirection);

            *PixelWrite++ =
            (
                (RoundF32ToU32(255.0f) << 24) |
                (RoundF32ToU32(255.0f * PixelColor.X) << 16) |
                (RoundF32ToU32(255.0f * PixelColor.Y) << 8) |
                (RoundF32ToU32(255.0f * PixelColor.Z) << 0)
            );

            // if (Y < 32)
            // {
            //     if (X < 500)
            //     {
            //         *PixelWrite++ = 0xFFFF0000;
            //     }
            //     else
            //     {
            //         *PixelWrite++ = 0xFFFFFF00;
            //     }
            // }
            // else
            // {
            //     *PixelWrite++ = 0xFF0000FF;
            // }
        }
    }

    WriteImage(OutputImage, "test.bmp");
    printf("--- raytracer done ---\n");

    return 0;
}
