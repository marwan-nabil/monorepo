#pragma once

#define PushStruct(Arena, DataType) (DataType *)PushOntoMemoryArena((Arena), sizeof(DataType))
#define PushArray(Arena, Count, DataType) (DataType *)PushOntoMemoryArena((Arena), (Count) * sizeof(DataType))
#define ZeroStruct(Struct) ZeroMemory(&(Struct), sizeof(Struct))

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

    entity_collision_rule *CollisionRulesTable[256];
    entity_collision_rule *FreeCollisionRulesListHead;

    entity_collision_mesh_group *NullCollisionMeshGroupTemplate;
    entity_collision_mesh_group *SwordCollisionMeshGroupTemplate;
    entity_collision_mesh_group *StairsCollisionMeshGroupTemplate;
    entity_collision_mesh_group *PlayerCollisionMeshGroupTemplate;
    entity_collision_mesh_group *MonsterCollisionMeshGroupTemplate;
    entity_collision_mesh_group *StandardRoomCollisionMeshGroupTemplate;
    entity_collision_mesh_group *WallCollisionMeshGroupTemplate;
    entity_collision_mesh_group *FamiliarCollisionMeshGroupTemplate;
};

struct render_piece
{
    loaded_bitmap *Bitmap;
    v4 Color;
    v3 Offset;
    f32 EntityJumpZCoefficient;
    v2 Dimensions;
};

struct render_peice_group
{
    u32 Count;
    render_piece Peices[8];
};