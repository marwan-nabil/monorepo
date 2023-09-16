// TODO: determine a good safety margin
#define MAX_CHUNK_DISTANCE_FROM_CENTER (INT32_MAX/64)
#define CHUNK_POSITION_UNINITIALIZED_VALUE INT32_MAX

inline b32
IsOffsetWithinInterval(f32 IntervalLength, f32 OffsetFromIntervalCenter)
{
    // TODO: fix fp math so this should be < & >
    f32 ToleranceEpsilon = 0.01f;
    f32 HalfIntervalLength = 0.5f * IntervalLength + ToleranceEpsilon;
    b32 Result =
    (
        (OffsetFromIntervalCenter >= -HalfIntervalLength) &&
        (OffsetFromIntervalCenter <= HalfIntervalLength)
    );
    return Result;
}

static b32
IsChunkCenterOffsetCanonical(world *World, v3 OffsetFromChunkCenter)
{
    b32 Result =
    (
        IsOffsetWithinInterval(World->ChunkDiameterInMeters.X, OffsetFromChunkCenter.X) &&
        IsOffsetWithinInterval(World->ChunkDiameterInMeters.Y, OffsetFromChunkCenter.Y) &&
        IsOffsetWithinInterval(World->ChunkDiameterInMeters.Z, OffsetFromChunkCenter.Z)
    );
    return Result;
}

inline world_position
InvalidWorldPosition()
{
    world_position Result = {};
    Result.ChunkX = CHUNK_POSITION_UNINITIALIZED_VALUE;
    return Result;
}

inline b32
IsWorldPositionValid(world_position WorldPosition)
{
    b32 Result = (WorldPosition.ChunkX != CHUNK_POSITION_UNINITIALIZED_VALUE);
    return Result;
}

inline b32
AreInTheSameChunk(world *World, world_position *A, world_position *B)
{
    Assert(IsChunkCenterOffsetCanonical(World, A->OffsetFromChunkCenter));
    Assert(IsChunkCenterOffsetCanonical(World, B->OffsetFromChunkCenter));

    b32 Result = 
    (
        (A->ChunkX == B->ChunkX) &&
        (A->ChunkY == B->ChunkY) &&
        (A->ChunkZ == B->ChunkZ)
    );

    return Result;
}

inline chunk *
GetChunk(world *World, memory_arena *MemoryArena, i32 ChunkX, i32 ChunkY, i32 ChunkZ)
{
    Assert(ChunkX > -MAX_CHUNK_DISTANCE_FROM_CENTER);
    Assert(ChunkY > -MAX_CHUNK_DISTANCE_FROM_CENTER);
    Assert(ChunkZ > -MAX_CHUNK_DISTANCE_FROM_CENTER);

    Assert(ChunkX < MAX_CHUNK_DISTANCE_FROM_CENTER);
    Assert(ChunkY < MAX_CHUNK_DISTANCE_FROM_CENTER);
    Assert(ChunkZ < MAX_CHUNK_DISTANCE_FROM_CENTER);

    chunk *Result = 0;

    u32 HashTableIndex = (u32)(19 * ChunkX + 7 * ChunkY + 3 * ChunkZ) & (u32)(ArrayLength(World->ChunksTable) - 1);
    Assert(HashTableIndex < ArrayLength(World->ChunksTable));

    chunk *CurrentChunk = World->ChunksTable + HashTableIndex;
    while (CurrentChunk)
    {
        if
        (
            (ChunkX == CurrentChunk->ChunkX) &&
            (ChunkY == CurrentChunk->ChunkY) &&
            (ChunkZ == CurrentChunk->ChunkZ)
        )
        {
            Result = CurrentChunk;
            break;
        }
        else
        {
            if (CurrentChunk->ChunkX == CHUNK_POSITION_UNINITIALIZED_VALUE)
            {
                CurrentChunk->ChunkX = ChunkX;
                CurrentChunk->ChunkY = ChunkY;
                CurrentChunk->ChunkZ = ChunkZ;
                CurrentChunk->NextChunk = 0;
                Result = CurrentChunk;
                break;
            }
            else
            {
                if (CurrentChunk->NextChunk)
                {
                    CurrentChunk = CurrentChunk->NextChunk;
                }
                else
                {
                    if (MemoryArena)
                    {
                        chunk *NewChunk = PushStruct(MemoryArena, chunk);

                        NewChunk->ChunkX = ChunkX;
                        NewChunk->ChunkY = ChunkY;
                        NewChunk->ChunkZ = ChunkZ;
                        NewChunk->NextChunk = 0;

                        CurrentChunk->NextChunk = NewChunk;
                        Result = NewChunk;
                        break;
                    }
                    else
                    {
                        Result = 0;
                        break;
                    }
                }
            }
        }
    }

    return Result;
}

inline void
CanonicalizeIntervalIndexAndOffset(f32 IntervalLength, i32 *IntervalIndex, f32 *OffsetFromIntervalCenter)
{
    // TODO: fix this buggy mess
    i32 IntervalIndexOffset = RoundF32ToI32(*OffsetFromIntervalCenter / IntervalLength);

    *IntervalIndex += IntervalIndexOffset;
    *OffsetFromIntervalCenter -= IntervalIndexOffset * IntervalLength;

    Assert(IsOffsetWithinInterval(IntervalLength, *OffsetFromIntervalCenter));
}

inline world_position
MapIntoWorldPosition(world *World, world_position BasePosition, v3 OffsetFromBase)
{
    world_position Result = BasePosition;
    Result.OffsetFromChunkCenter += OffsetFromBase;
    CanonicalizeIntervalIndexAndOffset(World->ChunkDiameterInMeters.X, &Result.ChunkX, &Result.OffsetFromChunkCenter.X);
    CanonicalizeIntervalIndexAndOffset(World->ChunkDiameterInMeters.Y, &Result.ChunkY, &Result.OffsetFromChunkCenter.Y);
    CanonicalizeIntervalIndexAndOffset(World->ChunkDiameterInMeters.Z, &Result.ChunkZ, &Result.OffsetFromChunkCenter.Z);
    return Result;
}

inline v3
SubtractPositions(world *World, world_position *A, world_position *B)
{
    v3 Result = V3
    (
        (f32)A->ChunkX - (f32)B->ChunkX,
        (f32)A->ChunkY - (f32)B->ChunkY,
        (f32)A->ChunkZ - (f32)B->ChunkZ
    );

    Result = Hadamard(World->ChunkDiameterInMeters, Result) + (A->OffsetFromChunkCenter - B->OffsetFromChunkCenter);

    return Result;
}

inline void
InitializeWorld(world *World, f32 TileSideInMeters, f32 TileDepthInMeters)
{
    World->TileSideInMeters = TileSideInMeters;
    World->TileDepthInMeters = TileDepthInMeters;
    World->ChunkDiameterInMeters.X = (f32)TILES_PER_CHUNK * TileSideInMeters;
    World->ChunkDiameterInMeters.Y = (f32)TILES_PER_CHUNK * TileSideInMeters;
    World->ChunkDiameterInMeters.Z = TileDepthInMeters;
    World->StorageEntitiesIndicesBlocksFreeListHead = 0;

    for (u32 Index = 0; Index < ArrayLength(World->ChunksTable); Index++)
    {
        World->ChunksTable[Index].ChunkX = CHUNK_POSITION_UNINITIALIZED_VALUE;
        World->ChunksTable[Index].FirstStorageEntitiesIndicesBlock.StorageEntitiesCount = 0;
        World->ChunksTable[Index].FirstStorageEntitiesIndicesBlock.NextBlock = 0;
    }
}

static void
RawChangeStorageEntityLocationInWorld(world *World, memory_arena *MemoryArena, u32 StorageIndex, world_position *OldPosition, world_position *NewPosition)
{
    Assert(!OldPosition || IsWorldPositionValid(*OldPosition));
    Assert(!NewPosition || IsWorldPositionValid(*NewPosition));
    Assert(MemoryArena);

    if (OldPosition && NewPosition && AreInTheSameChunk(World, OldPosition, NewPosition))
    {
    }
    else
    {
        if (OldPosition)
        {
            chunk *OldLocationChunk = GetChunk(World, 0, OldPosition->ChunkX, OldPosition->ChunkY, OldPosition->ChunkZ);
            Assert(OldLocationChunk);

            storage_entities_indices_block *FirstBlockInChunk = &OldLocationChunk->FirstStorageEntitiesIndicesBlock;
            for
            (
                storage_entities_indices_block *CurrentIndicesBlock = FirstBlockInChunk; 
                CurrentIndicesBlock; 
                CurrentIndicesBlock = CurrentIndicesBlock->NextBlock
            )
            {
                b32 OuterBreakFlag = FALSE;
                for
                (
                    u32 StorageEntityIndexIndex = 0; 
                    StorageEntityIndexIndex < CurrentIndicesBlock->StorageEntitiesCount; 
                    StorageEntityIndexIndex++
                )
                {
                    if (CurrentIndicesBlock->StorageEntitiesIndices[StorageEntityIndexIndex] == StorageIndex)
                    {
                        Assert(FirstBlockInChunk->StorageEntitiesCount > 0);

                        CurrentIndicesBlock->StorageEntitiesIndices[StorageEntityIndexIndex] =
                            FirstBlockInChunk->StorageEntitiesIndices[--FirstBlockInChunk->StorageEntitiesCount];

                        if ((FirstBlockInChunk->StorageEntitiesCount == 0) && (FirstBlockInChunk->NextBlock))
                        {
                            storage_entities_indices_block *SecondBlock = FirstBlockInChunk->NextBlock;
                            *FirstBlockInChunk = *SecondBlock;

                            SecondBlock->NextBlock = World->StorageEntitiesIndicesBlocksFreeListHead;
                            World->StorageEntitiesIndicesBlocksFreeListHead = SecondBlock;
                        }

                        OuterBreakFlag = TRUE;
                        break;
                    }
                }

                if (OuterBreakFlag) break;
            }
        }

        if (NewPosition)
        {
            chunk *NewLocationChunk = GetChunk(World, MemoryArena, NewPosition->ChunkX, NewPosition->ChunkY, NewPosition->ChunkZ);
            Assert(NewLocationChunk);
            storage_entities_indices_block *FirstBlock = &NewLocationChunk->FirstStorageEntitiesIndicesBlock;

            if (FirstBlock->StorageEntitiesCount == ArrayLength(FirstBlock->StorageEntitiesIndices))
            {
                storage_entities_indices_block *NewBlock;
                if (World->StorageEntitiesIndicesBlocksFreeListHead)
                {
                    NewBlock = World->StorageEntitiesIndicesBlocksFreeListHead;
                    World->StorageEntitiesIndicesBlocksFreeListHead = World->StorageEntitiesIndicesBlocksFreeListHead->NextBlock;
                }
                else
                {
                    NewBlock = PushStruct(MemoryArena, storage_entities_indices_block);
                }

                *NewBlock = *FirstBlock;

                FirstBlock->NextBlock = NewBlock;
                FirstBlock->StorageEntitiesCount = 0;
            }

            Assert(FirstBlock->StorageEntitiesCount < ArrayLength(FirstBlock->StorageEntitiesIndices));
            FirstBlock->StorageEntitiesIndices[FirstBlock->StorageEntitiesCount++] = StorageIndex;
        }
    }
}

static void
ChangeStorageEntityLocationInWorld
(
    world *World, memory_arena *MemoryArena, u32 StorageIndex, 
    storage_entity *StorageEntity, world_position *NewWorldPosition
)
{
    world_position *OldPosition = 0;
    if
    (
        !IsFlagSet(&StorageEntity->Entity, EF_NON_SPATIAL) &&
        IsWorldPositionValid(StorageEntity->WorldPosition)
    )
    {
        OldPosition = &StorageEntity->WorldPosition;
    }

    world_position *NewPosition = 0;
    if (IsWorldPositionValid(*NewWorldPosition))
    {
        NewPosition = NewWorldPosition;
    }

    RawChangeStorageEntityLocationInWorld(World, MemoryArena, StorageIndex, OldPosition, NewPosition);

    if (NewPosition)
    {
        StorageEntity->WorldPosition = *NewPosition;
        ClearFlags(&StorageEntity->Entity, EF_NON_SPATIAL);
    }
    else
    {
        StorageEntity->WorldPosition = InvalidWorldPosition();
        SetFlags(&StorageEntity->Entity, EF_NON_SPATIAL);
    }
}

static world_position
GetWorldPositionFromTilePosition(world *World, i32 AbsTileX, i32 AbsTileY, i32 AbsTileZ, v3 OffsetFromTileCenter)
{
    // TODO: fix buggy Z handling here

    world_position BasePosition = {};

    v3 TileDiameter = V3(World->TileSideInMeters, World->TileSideInMeters, World->TileDepthInMeters);
    v3 AbsoluteWorldOffset = 
        Hadamard(TileDiameter, V3((f32)AbsTileX, (f32)AbsTileY, (f32)AbsTileZ)) + 
        OffsetFromTileCenter;
    //v3 AbsoluteWorldOffset = Hadamard(TileDiameter, V3((f32)AbsTileX, (f32)AbsTileY, (f32)AbsTileZ)) + OffsetFromTileCenter + 0.5f * TileDiameter;

    world_position Result = MapIntoWorldPosition(World, BasePosition, AbsoluteWorldOffset);

    Assert(IsChunkCenterOffsetCanonical(World, Result.OffsetFromChunkCenter));

    return Result;
}
