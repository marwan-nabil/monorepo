#pragma once

struct memory_arena
{
    size_t Size;
    size_t Used;
    u8 *Base;
};

struct loaded_bitmap
{
    i32 Width;
    i32 Height;
    u32 *Pixels;
};

struct hero_bitmap_group
{
    v2 Alignment;
    loaded_bitmap Head;
    loaded_bitmap Cape;
    loaded_bitmap Torso;
};

struct controlled_hero_input
{
    u32 HeroEntityStorageIndex;
    v3 InputAcceleration;
    v2 InputSwordDirection;
    f32 InputJumpVelocity;
};

struct game_state
{
    memory_arena WorldArena;
    world *World;

    loaded_bitmap BackDropBitMap;
    loaded_bitmap ShadowBitMap;
    loaded_bitmap TreeBitMap;
    loaded_bitmap SwordBitMap;
    loaded_bitmap StairWellBitMap;
    hero_bitmap_group HeroBitmapGroups[4];

    f32 PixelsToMetersRatio;

    world_position CameraPosition;
    u32 StorageIndexOfEntityThatCameraFollows;

    controlled_hero_input ControllerToHeroInputMap[ArrayLength(((game_input *)0)->ControllerStates)];

    pairwise_collision_rule *CollisionRulesTable[256];
    pairwise_collision_rule *FreeCollisionRulesListHead;

    entity_collision_mesh_group *NullCollisionMeshGroupTemplate;
    entity_collision_mesh_group *SwordCollisionMeshGroupTemplate;
    entity_collision_mesh_group *StairsCollisionMeshGroupTemplate;
    entity_collision_mesh_group *PlayerCollisionMeshGroupTemplate;
    entity_collision_mesh_group *MonsterCollisionMeshGroupTemplate;
    entity_collision_mesh_group *StandardRoomCollisionMeshGroupTemplate;
    entity_collision_mesh_group *WallCollisionMeshGroupTemplate;
    entity_collision_mesh_group *FamiliarCollisionMeshGroupTemplate;
};

struct entity_render_peice
{
    loaded_bitmap *Bitmap;
    v4 Color;
    v3 Offset;
    f32 EntityJumpZCoefficient;
    v2 Dimensions;
};

struct entity_render_peice_group
{
    u32 Count;
    entity_render_peice Peices[8];
};

#define PushStruct(Arena, DataType) (DataType *)PushSize_((Arena), sizeof(DataType))
#define PushArray(Arena, Count, DataType) (DataType *)PushSize_((Arena), (Count) * sizeof(DataType))

inline void *
PushSize_(memory_arena *Arena, size_t PushSize)
{
    Assert((Arena->Used + PushSize) <= Arena->Size);
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += PushSize;
    return Result;
}

inline storage_entity *
GetStorageEntity(game_state *GameState, u32 StorageIndex)
{
    storage_entity *Result = 0;

    if ((StorageIndex > 0) && (StorageIndex < GameState->World->StorageEntitiesCount))
    {
        Result = GameState->World->StorageEntities + StorageIndex;
    }

    return Result;
}

#define ZeroStruct(Struct) ZeroSize(&(Struct), sizeof(Struct))

inline void
ZeroSize(void *Base, size_t Size)
{
    u8 *Byte = (u8 *)Base;
    while (Size--)
    {
        *Byte++ = 0;
    }
}

static void
AddPairwiseCollisionRule(game_state *GameState, u32 FirstEntityStorageIndex, u32 SecondEntityStorageIndex, b32 ShouldCollide);

static void
ClearAllPairwiseCollisionRule(game_state *GameState, u32 StorageIndex);