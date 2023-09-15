#if !defined(HANDMADE_ENTITY_H)

#define INVALID_ENTITY_POSITION V3(100000.0f, 100000.0f, 100000.0f)

struct entity;

struct world_position
{
    i32 ChunkX, ChunkY, ChunkZ;
    v3 OffsetFromChunkCenter;
};

union entity_reference
{
    entity *Entity;
    u32 StorageIndex;
};

#define HIT_POINT_SUB_COUNT 4
struct hit_point
{
    u8 Flags;
    u8 FilledAmount;
};

enum entity_type
{
    ET_NULL,
    ET_SPACE,
    ET_WALL,
    ET_HERO,
    ET_FAMILIAR,
    ET_MONSTER,
    ET_SWORD,
    ET_STAIRS
};

enum entity_flags
{
    EF_COLLIDES = (1 << 0u),
    EF_NON_SPATIAL = (1 << 1u),
    EF_MOVEABLE = (1 << 2u),
    EF_Z_SUPPORTED = (1 << 3u),
    EF_TRAVERSABLE = (1 << 4u),

    EF_SIMULATING = (1 << 30u)
};

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

struct entity
{
    entity_type Type;
    u32 Flags;
    b32 CanUpdate;

    v3 Position;
    v3 Velocity;
    
    f32 BobbingSinParameter;
    f32 MovementDistanceLimit;
    
    u32 StorageIndex;

    entity_collision_mesh_group *CollisionMeshGroup;
    
    i32 DiffAbsTileZ; // unused

    u32 HitPointsMax;
    hit_point HitPoints[16];

    u32 BitmapFacingDirection;

    entity_reference SwordEntityReference;

    // NOTE: only for the stairwell
    f32 WalkableHeight;
    v2 WalkableDiameter;
};

struct storage_entity
{
    entity Entity;
    world_position WorldPosition;
};

struct pairwise_collision_rule
{
    u32 EntityAStorageIndex;
    u32 EntityBStorageIndex;
    b32 CanCollide;
    pairwise_collision_rule *Next;
};

inline b32
IsFlagSet(entity *Entity, u32 Flag)
{
    b32 Result = ((Entity->Flags & Flag) != 0);
    return Result;
}

inline b32
AreAnyOfFlagsSet(entity *Entity, u32 Flags)
{
    b32 Result = ((Entity->Flags & Flags) != 0);
    return Result;
}

inline void
SetFlags(entity *Entity, u32 Flags)
{
    Entity->Flags |= Flags;
}

inline void
ClearFlags(entity *Entity, u32 Flags)
{
    Entity->Flags &= ~Flags;
}

inline void
ClearAllFlags(entity *Entity)
{
    Entity->Flags = 0;
}

#define HANDMADE_ENTITY_H
#endif