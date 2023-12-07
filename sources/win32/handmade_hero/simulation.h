#pragma once

struct entity_table_entry
{
    entity *Entity;
    u32 StorageIndex;
};

struct simulation_region
{
    world *World;

    entity_world_position Origin;
    rectangle3 SimulationBounds;
    rectangle3 UpdateBounds;

    f32 MaxEntityRadius;
    f32 MaxEntityVelocity;

    u32 MaxEntityCount;
    u32 CurrentEntityCount;
    entity *Entities;

    entity_table_entry EntityTable[4096];
};

inline void LoadEntityReference(game_state *GameState, simulation_region *SimulationRegion, entity_reference *EntityReference);