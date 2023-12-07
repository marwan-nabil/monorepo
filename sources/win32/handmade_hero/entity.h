#pragma once

#define ENTITY_INIFINTE_MOVEMENT_DISTANCE 10000.0f
#define INVALID_ENTITY_POSITION V3(100000.0f, 100000.0f, 100000.0f)
#define ENTITY_HIT_POINT_SUBCOUNT 4

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

enum entity_flag
{
    EF_COLLIDES = (1 << 0u),
    EF_NON_SPATIAL = (1 << 1u),
    EF_MOVEABLE = (1 << 2u),
    EF_Z_SUPPORTED = (1 << 3u),
    EF_TRAVERSABLE = (1 << 4u),

    EF_SIMULATING = (1 << 30u)
};

struct entity_movement_parameters
{
    b32 NormalizeAcceleration;
    f32 DragInXYPlane;
    f32 SpeedInXYPlane;
};

struct entity_world_position
{
    i32 ChunkX, ChunkY, ChunkZ;
    v3 OffsetFromChunkCenter;
};

struct entity_hit_point
{
    u8 Flags;
    u8 FilledAmount;
};

struct entity;

union entity_reference
{
    entity *Entity;
    u32 StorageIndex;
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
    entity_hit_point HitPoints[16];

    u32 BitmapFacingDirection;

    entity_reference SwordEntityReference;

    // NOTE: only for the stairwell
    f32 WalkableHeight;
    v2 WalkableDiameter;
};

struct storage_entity
{
    entity Entity;
    entity_world_position WorldPosition;
};

struct add_storage_entity_result
{
    u32 StorageIndex;
    storage_entity *StorageEntity;
};