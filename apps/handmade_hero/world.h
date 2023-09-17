#pragma once

#define TILES_PER_CHUNK 16
#define MAX_CHUNK_DISTANCE_FROM_CENTER (INT32_MAX / 64)
#define CHUNK_POSITION_UNINITIALIZED_VALUE INT32_MAX

struct storage_entity_indices_block
{
    u32 StorageEntityIndicesCount;
    u32 StorageEntityIndices[16];
    storage_entity_indices_block *NextBlock;
};

struct chunk
{
    i32 ChunkX;
    i32 ChunkY;
    i32 ChunkZ;
    storage_entity_indices_block FirstStorageEntitiesIndicesBlock;
    chunk *NextChunk;
};

struct world
{
    f32 TileSideInMeters;
    f32 TileDepthInMeters;
    v3 ChunkDiameterInMeters;

    chunk ChunksTable[4096];

    storage_entity_indices_block *StorageEntitiesIndicesBlocksFreeListHead;

    u32 StorageEntityCount;
    storage_entity StorageEntities[100000];
};

struct world_position
{
    i32 ChunkX, ChunkY, ChunkZ;
    v3 OffsetFromChunkCenter;
};