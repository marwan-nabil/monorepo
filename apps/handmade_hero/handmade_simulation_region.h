#if !defined(HANDMADE_SIMULATION_REGION_H)

#include "handmade_platform.h"
#include "handmade_math.h"

#include "handmade_entity.h"
#include "handmade_world.h"

struct move_spec
{
    b32 NormalizeAcceleration;
    f32 DragInXYPlane;
    f32 SpeedInXYPlane;
};

struct entity_table_entry
{
    entity *Entity;
    u32 StorageIndex;
};

struct simulation_region
{
    world *World;
    
    world_position Origin;
    rectangle3 SimulationBounds;
    rectangle3 UpdateBounds;

    f32 MaxEntityRadius;
    f32 MaxEntityVelocity;

    u32 MaxEntityCount;
    u32 CurrentEntityCount;
    entity *Entities;

    entity_table_entry EntityTable[4096];
};

#define HANDMADE_SIMULATION_REGION_H
#endif