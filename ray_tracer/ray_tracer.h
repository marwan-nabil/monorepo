#pragma once

#define BOUNCES_PER_RAY 8
#define RAYS_PER_PIXEL 256
#define RAY_BATCHES_PER_PIXEL (RAYS_PER_PIXEL / SIMD_NUMBEROF_LANES)

struct image_u32
{
    u32 WidthInPixels;
    u32 HeightInPixels;
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
    f32 HalfWidth;
    f32 HalfHeight;
    f32 HalfPixelWidth;
    f32 HalfPixelHeight;
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
    u8 CoreCount;

    u32 TileWidthInPixels;
    u32 TileHeightInPixels;
    u32 TileCountX;
    u32 TileCountY;
    u32 TotalTileCount;

    f32 HitDistanceLowerLimit;
    f32 ToleranceToZero;
};

struct work_order
{
    world *World;
    image_u32 *Image;
    rendering_parameters *RenderingParameters;
    random_series_lane Entropy;

    u32 StartPixelX;
    u32 StartPixelY;
    u32 EndPixelX;
    u32 EndPixelY;
};

struct work_queue
{
    u32 WorkOrderCount;
    work_order *WorkOrders;

    volatile u64 TotalRayBouncesComputed;
    volatile u64 TotalTilesDone;
    volatile u64 NextWorkOrderIndex;
};