#pragma once

#define TILES_PER_CHUNK 16

struct storage_entities_indices_block
{
    u32 StorageEntitiesCount;
    u32 StorageEntitiesIndices[16];
    storage_entities_indices_block *NextBlock;
};

struct chunk
{
    i32 ChunkX, ChunkY, ChunkZ;
    storage_entities_indices_block FirstStorageEntitiesIndicesBlock;
    chunk *NextChunk;
};

struct world
{
    f32 TileSideInMeters;
    f32 TileDepthInMeters;
    v3 ChunkDiameterInMeters;

    chunk ChunksTable[4096];

    storage_entities_indices_block *StorageEntitiesIndicesBlocksFreeListHead;

    u32 StorageEntitiesCount;
    storage_entity StorageEntities[100000];
};