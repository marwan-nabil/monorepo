#pragma once

struct entity_collision_mesh
{
    v3 Diameter;
    v3 Offset;
};

struct entity_collision_mesh_group
{
    u32 MeshCount;
    entity_collision_mesh *Meshes;
    entity_collision_mesh TotalCollisionMesh;
};

struct entity_collision_rule
{
    u32 EntityAStorageIndex;
    u32 EntityBStorageIndex;
    b32 CanCollide;
    entity_collision_rule *Next;
};

struct game_state;
struct entity;

static b32 CanEntitiesCollide(game_state *GameState, entity *A, entity *B);
static b32 ProcessEntityCollision(game_state *GameState, entity *MovingEntity, entity *StaticEntity);