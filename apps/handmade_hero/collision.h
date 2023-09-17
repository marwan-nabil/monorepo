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