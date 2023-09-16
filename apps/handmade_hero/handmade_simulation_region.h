#pragma once

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