#pragma once

#pragma pack(push, 1)
struct bitmap_header
{
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
    u32 Size;
    i32 Width;
    i32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 SizeOfBitmap;
    i32 HorizontalResolution;
    i32 VerticalResolution;
    u32 ColorsUsed;
    u32 ColorsImportant;
};
#pragma pack(pop)

struct image_u32
{
    u32 Width;
    u32 Height;
    u32 *Pixels;
};

struct material
{
    f32 Specularity;
    v3 ReflectionColor;
    v3 EmmissionColor;
};

struct plane
{
    v3 Normal;
    f32 Distance;
    u32 MaterialIndex;
};

struct sphere
{
    v3 Position;
    f32 Radius;
    u32 MaterialIndex;
};

struct camera
{
    v3 Position;
    coordinate_set CoordinateSet;
};

struct film
{
    v3 Center;
    f32 DistanceFromCamera;
    f32 Width;
    f32 Height;
    f32 PixelWidth;
    f32 PixelHeight;
};

struct world
{
    coordinate_set CoordinateSet;
    camera Camera;
    film Film;

    material *Materials;
    u32 MaterialsCount;

    plane *Planes;
    u32 PlanesCount;

    sphere *Spheres;
    u32 SpheresCount;
};

struct rendering_parameters
{
    u64 TotalRayBouncesComputed;
    u32 TotalTilesDone;
    
    u8 CoreCount;

    u32 TileWidthInPixels;
    u32 TileHeightInPixels;
    u32 TileCountX;
    u32 TileCountY;
    u32 TotalTileCount;

    f32 HitDistanceLowerLimit;
    f32 ToleranceToZero;
};