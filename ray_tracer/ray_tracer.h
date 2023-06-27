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
    v3 Color;
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

struct world
{
    material *Materials;
    u32 MaterialsCount;

    plane *Planes;
    u32 PlanesCount;

    sphere *Spheres;
    u32 SpheresCount;
};
