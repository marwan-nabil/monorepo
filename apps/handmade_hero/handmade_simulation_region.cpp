#include "handmade_platform.h"
#include "handmade_math.h"
#include "handmade_intrinsics.h"
#include "handmade.h"
#include "handmade_world.h"
#include "handmade_simulation_region.h"

internal entity *
AddEntityToSimulation(game_state *GameState, simulation_region *SimulationRegion,
          u32 StorageIndex, storage_entity *SourceStorageEntity,
          v3 *InitialPositionInSimulationRegion);

internal entity_table_entry *
GetEntityHashtableEntry(simulation_region *SimulationRegion, u32 StorageIndex)
{
    Assert(StorageIndex);

    entity_table_entry *Result = 0;
    u32 InitialHashTableIndex = StorageIndex;
    for
    (
        u32 ProbingOffset = 0;
        ProbingOffset < ArrayCount(SimulationRegion->EntityTable);
        ProbingOffset++
    )
    {
        u32 HashTableIndex = (InitialHashTableIndex + ProbingOffset) & (ArrayCount(SimulationRegion->EntityTable) - 1);
        entity_table_entry *HashtableEntry = SimulationRegion->EntityTable + HashTableIndex;

        if ((HashtableEntry->StorageIndex == 0) || (HashtableEntry->StorageIndex == StorageIndex))
        {
            Result = HashtableEntry;
            break;
        }
    }

    return Result;
}

inline v3
GetSimulationRegionRelativePosition(simulation_region *SimulationRegion, storage_entity *StorageEntity)
{
    v3 Result;
    if (!IsFlagSet(&StorageEntity->Entity, EF_NON_SPATIAL))
    {
        Result = SubtractPositions(SimulationRegion->World, &StorageEntity->WorldPosition, &SimulationRegion->Origin);
    }
    else
    {
        Result = INVALID_ENTITY_POSITION;
    }
    return Result;
}

inline entity *
GetEntity(simulation_region *SimulationRegion, u32 StorageIndex)
{
    entity_table_entry *EntityHashTableEntry =
        GetEntityHashtableEntry(SimulationRegion, StorageIndex);
    entity *Result = EntityHashTableEntry->Entity;
    return Result;
}

inline void
LoadEntityReference(game_state *GameState, simulation_region *SimulationRegion, entity_reference *EntityReference)
{
    if (EntityReference->StorageIndex)
    {
        entity_table_entry *EntityHashTableEntry =
            GetEntityHashtableEntry(SimulationRegion, EntityReference->StorageIndex);
        
        if (EntityHashTableEntry->Entity == 0)
        {
            storage_entity *ReferencedStorageEntity = GetStorageEntity(GameState, EntityReference->StorageIndex);
            v3 SimulationRegionRelativePosition = GetSimulationRegionRelativePosition(SimulationRegion, ReferencedStorageEntity);

            EntityHashTableEntry->StorageIndex = EntityReference->StorageIndex;
            EntityHashTableEntry->Entity =
                AddEntityToSimulation(GameState, SimulationRegion, EntityReference->StorageIndex,
                          ReferencedStorageEntity, &SimulationRegionRelativePosition);
        }

        EntityReference->Entity = EntityHashTableEntry->Entity;
    }
}

inline void
StoreEntityReference(entity_reference *EntityReference)
{
    if (EntityReference->Entity)
    {
        EntityReference->StorageIndex = EntityReference->Entity->StorageIndex;
    }
}

internal entity *
RawAddEntityToSimulation(game_state *GameState, simulation_region *SimulationRegion, u32 StorageIndex, storage_entity *SourceStorageEntity)
{
    Assert(StorageIndex);

    entity *Result = 0;
    entity_table_entry *HashtableEntry = GetEntityHashtableEntry(SimulationRegion, StorageIndex);

    if (HashtableEntry->Entity == 0)
    {
        if (SimulationRegion->CurrentEntityCount < SimulationRegion->MaxEntityCount)
        {
            Result = SimulationRegion->Entities + SimulationRegion->CurrentEntityCount++;

            HashtableEntry->StorageIndex = StorageIndex;
            HashtableEntry->Entity = Result;

            if (SourceStorageEntity)
            {
                *Result = SourceStorageEntity->Entity;
                LoadEntityReference(GameState, SimulationRegion, &Result->SwordEntityReference);

                Assert(!IsFlagSet(&SourceStorageEntity->Entity, EF_SIMULATING));
                SetFlags(&SourceStorageEntity->Entity, EF_SIMULATING);
            }

            Result->StorageIndex = StorageIndex;
            Result->CanUpdate = false;
        }
        else
        {
            InvalidCodepath;
        }
    }

    return Result;
}

inline b32
DoesEntityCollisionMeshOverlapRectangle(v3 EntityPosition, entity_collision_mesh CollisionMesh, rectangle3 TestingRectangle)
{
    rectangle3 ExpandedTestingRectangle = ExpandRectangle(TestingRectangle, CollisionMesh.Diameter);
    b32 Result = IsInRectangle(ExpandedTestingRectangle, EntityPosition + CollisionMesh.Offset);
    return Result;
}

internal entity *
AddEntityToSimulation(game_state *GameState, simulation_region *SimulationRegion,
                      u32 StorageIndex, storage_entity *SourceStorageEntity,
                      v3 *InitialPositionInSimulationRegion)
{
    entity *Result = RawAddEntityToSimulation(GameState, SimulationRegion, StorageIndex, SourceStorageEntity);
    if (Result)
    {
        if (InitialPositionInSimulationRegion)
        {
            Result->Position = *InitialPositionInSimulationRegion;
            Result->CanUpdate = DoesEntityCollisionMeshOverlapRectangle
            (
                Result->Position,
                Result->CollisionMeshGroup->TotalCollisionMesh,
                SimulationRegion->UpdateBounds
            );
        }
        else
        {
            Result->Position =
                GetSimulationRegionRelativePosition(SimulationRegion, SourceStorageEntity);
        }
    }
    return Result;
}

internal simulation_region *
BeginSimulation(game_state *GameState, world *World, memory_arena *SimulationArena, 
                world_position RegionOrigin, rectangle3 UpdateBounds, f32 TimeDelta)
{
    simulation_region *SimulationRegion = PushStruct(SimulationArena, simulation_region);
    ZeroStruct(SimulationRegion->EntityTable);

    SimulationRegion->World = World;
    SimulationRegion->Origin = RegionOrigin;

    SimulationRegion->MaxEntityRadius = 5.0f;
    SimulationRegion->MaxEntityVelocity = 30.0f;
    f32 SimulationBoundsSafetyMargin = SimulationRegion->MaxEntityRadius + SimulationRegion->MaxEntityVelocity * TimeDelta;

    SimulationRegion->UpdateBounds =
        ExpandRectangle(UpdateBounds, 
                        2 * V3(SimulationRegion->MaxEntityRadius, SimulationRegion->MaxEntityRadius, SimulationRegion->MaxEntityRadius));

    SimulationRegion->SimulationBounds =
        ExpandRectangle(SimulationRegion->UpdateBounds,  // NOTE: or just the given UpdateBounds directly ?
                        2 * V3(SimulationBoundsSafetyMargin, SimulationBoundsSafetyMargin, SimulationBoundsSafetyMargin));

    SimulationRegion->MaxEntityCount = 4096;
    SimulationRegion->CurrentEntityCount = 0;
    SimulationRegion->Entities = PushArray(SimulationArena, SimulationRegion->MaxEntityCount, entity);

    world_position SimulationAreaMinChunkPosition = MapIntoWorldPosition(World, SimulationRegion->Origin, GetMinCorner(SimulationRegion->SimulationBounds));
    world_position SimulationAreaMaxChunkPosition = MapIntoWorldPosition(World, SimulationRegion->Origin, GetMaxCorner(SimulationRegion->SimulationBounds));

    for (i32 ChunkZ = SimulationAreaMinChunkPosition.ChunkZ; ChunkZ <= SimulationAreaMaxChunkPosition.ChunkZ; ChunkZ++)
    {
        for (i32 ChunkY = SimulationAreaMinChunkPosition.ChunkY; ChunkY <= SimulationAreaMaxChunkPosition.ChunkY; ChunkY++)
        {
            for (i32 ChunkX = SimulationAreaMinChunkPosition.ChunkX; ChunkX <= SimulationAreaMaxChunkPosition.ChunkX; ChunkX++)
            {
                chunk *CurrentChunk = GetChunk(World, 0, ChunkX, ChunkY, ChunkZ);
                if (CurrentChunk)
                {
                    for
                    (
                        storage_entities_indices_block *Block = &CurrentChunk->FirstStorageEntitiesIndicesBlock;
                        Block;
                        Block = Block->NextBlock
                    )
                    {
                        for (u32 StorageEntityIndexIndex = 0; StorageEntityIndexIndex < Block->StorageEntitiesCount; StorageEntityIndexIndex++)
                        {
                            u32 StorageIndex = Block->StorageEntitiesIndices[StorageEntityIndexIndex];
                            storage_entity *StorageEntity = GetStorageEntity(GameState, StorageIndex);

                            if (!IsFlagSet(&StorageEntity->Entity, EF_NON_SPATIAL))
                            {
                                v3 Position = GetSimulationRegionRelativePosition(SimulationRegion, StorageEntity);
                                if
                                (
                                    DoesEntityCollisionMeshOverlapRectangle
                                    (
                                        Position, 
                                        StorageEntity->Entity.CollisionMeshGroup->TotalCollisionMesh,
                                        SimulationRegion->SimulationBounds
                                    )
                                )
                                {
                                    AddEntityToSimulation(GameState, SimulationRegion, StorageIndex, StorageEntity, &Position);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return SimulationRegion;
}

internal void 
EndSimulation(simulation_region *SimulationRegion, game_state *GameState)
{
    entity *Entity = SimulationRegion->Entities;
    for
    (
        u32 EntityIndex = 0;
        EntityIndex < SimulationRegion->CurrentEntityCount;
        EntityIndex++, Entity++
    )
    {
        storage_entity *StorageEntity = GetStorageEntity(GameState, Entity->StorageIndex);

        Assert(IsFlagSet(&StorageEntity->Entity, EF_SIMULATING));
        StorageEntity->Entity = *Entity;
        Assert(!IsFlagSet(&StorageEntity->Entity, EF_SIMULATING));

        StoreEntityReference(&StorageEntity->Entity.SwordEntityReference);

        world_position NewWorldPosition;
        if (IsFlagSet(Entity, EF_NON_SPATIAL))
        {
            NewWorldPosition = InvalidWorldPosition();
        }
        else
        {
            NewWorldPosition =
                MapIntoWorldPosition(GameState->World, SimulationRegion->Origin, Entity->Position);
        }

        ChangeStorageEntityLocationInWorld(GameState->World, &GameState->WorldArena, Entity->StorageIndex,
                                           StorageEntity, &NewWorldPosition);

        if (Entity->StorageIndex == GameState->StorageIndexOfEntityThatCameraFollows)
        {
            world_position NewCameraPosition;
#if 0

            if (MovingEntity->Position.X > (0.5f * TilesPerScreenWidth * TileSideInMeters))
            {
                // new camera position = old camera position + 
                NewCameraPosition.OffsetFromChunkCenter += V2((f32)TileSideInMeters * TilesPerScreenWidth, 0.0f);
            }
            if (StorageEntityThatCameraFollows.High->PositionRelativeToCamera.X < -(0.5f * TilesPerScreenWidth * TileSideInMeters))
            {
                NewCameraPosition.OffsetFromChunkCenter -= V2((f32)TileSideInMeters * TilesPerScreenWidth, 0.0f);
            }
            if (StorageEntityThatCameraFollows.High->PositionRelativeToCamera.SquareCenterRelativeCollisionPointY > (0.5f * TilesPerScreenHeight * TileSideInMeters))
            {
                NewCameraPosition.OffsetFromChunkCenter += V2(0.0f, (f32)TileSideInMeters * TilesPerScreenHeight);
            }
            if (StorageEntityThatCameraFollows.High->PositionRelativeToCamera.SquareCenterRelativeCollisionPointY < -(0.5f * TilesPerScreenHeight * TileSideInMeters))
            {
                NewCameraPosition.OffsetFromChunkCenter -= V2(0.0f, (f32)TileSideInMeters * TilesPerScreenHeight);
            }
#else
            NewCameraPosition = StorageEntity->WorldPosition;
            NewCameraPosition.OffsetFromChunkCenter.Z = GameState->CameraPosition.OffsetFromChunkCenter.Z;
#endif
            GameState->CameraPosition = NewCameraPosition;
        }
    }
}

internal b32
TestWall
(
    f32 SquareCenterRelativeWallX,
    f32 SquareCenterToMovingEntityOriginalPositionX, f32 SquareCenterToMovingEntityOriginalPositionY, 
    f32 MovingEntityMovementVectorX, f32 MovingEntityMovementVectorY, 
    f32 *OriginalMinimalTParameter, f32 SquareCenterRelativeMinimumOrthogonalWallY, f32 SquareCenterRelativeMaximumOrthogonalWallY
)
{
    b32 DidMovingEntityHitWall = false;

    if (MovingEntityMovementVectorX != 0.0f)
    {
        f32 TParameter = (SquareCenterRelativeWallX - SquareCenterToMovingEntityOriginalPositionX) / MovingEntityMovementVectorX;
        f32 SquareCenterRelativeCollisionPointY = SquareCenterToMovingEntityOriginalPositionY + TParameter * MovingEntityMovementVectorY;
        if 
        (
            (SquareCenterRelativeCollisionPointY >= SquareCenterRelativeMinimumOrthogonalWallY) && 
            (SquareCenterRelativeCollisionPointY <= SquareCenterRelativeMaximumOrthogonalWallY)
        )
        {
            if ((TParameter >= 0) && (TParameter < *OriginalMinimalTParameter))
            {
                f32 ToleranceEpsilon = 0.001f;
                *OriginalMinimalTParameter = Maximum(0, TParameter - ToleranceEpsilon);
                DidMovingEntityHitWall = true;
            }
        }
    }
    
    return DidMovingEntityHitWall;
}

inline void
SortEntityPointersByEntityTypes(entity **A, entity **B)
{
    if ((*A)->Type > (*B)->Type)
    {
        entity *SwappingTemporary = *A;
        *A = *B;
        *B = SwappingTemporary;
    }
}

internal b32
ProcessCollisionEvent(game_state *GameState, entity *MovingEntity, entity *StaticEntity)
{
    b32 MovingEntityShouldStopOnCollision = false;

    if (MovingEntity->Type == ET_SWORD)
    {
        AddPairwiseCollisionRule(GameState, MovingEntity->StorageIndex, StaticEntity->StorageIndex, false);
        MovingEntityShouldStopOnCollision = false;
    }
    else
    {
        MovingEntityShouldStopOnCollision = true;
    }

    entity *LowerTypeEntity = MovingEntity;
    entity *HigherTypeEntity = StaticEntity;
    SortEntityPointersByEntityTypes(&LowerTypeEntity, &HigherTypeEntity);

    if ((LowerTypeEntity->Type == ET_MONSTER) && (HigherTypeEntity->Type == ET_SWORD))
    {
        if (LowerTypeEntity->HitPointsMax > 0)
        {
            LowerTypeEntity->HitPointsMax--;
        }
    }

    return MovingEntityShouldStopOnCollision;
}

internal b32
CanEntitiesCollide(game_state *GameState, entity *A, entity *B)
{
    b32 Result = false;

    if (A != B)
    {
        SortEntityPointersByEntityTypes(&A, &B);

        if (IsFlagSet(A, EF_COLLIDES) && IsFlagSet(B, EF_COLLIDES))
        {
            if (!IsFlagSet(A, EF_NON_SPATIAL) && !IsFlagSet(B, EF_NON_SPATIAL))
            {
                Result = true;
            }

            u32 HashTableIndex = A->StorageIndex & (ArrayCount(GameState->CollisionRulesTable) - 1);
            for
            (
                pairwise_collision_rule *CurrentRule = GameState->CollisionRulesTable[HashTableIndex];
                CurrentRule;
                CurrentRule = CurrentRule->Next
            )
            {
                if
                (
                    (CurrentRule->EntityAStorageIndex == A->StorageIndex) &&
                    (CurrentRule->EntityBStorageIndex == B->StorageIndex)
                )
                {
                    Result = CurrentRule->CanCollide;
                    break;
                }
            }
        }
    }

    return Result;
}

internal b32
CanEntitiesOverlap(game_state *GameState, entity *MovingEntity, entity *TestEntity)
{
    b32 Result = false;
    if 
    (
        (MovingEntity != TestEntity) &&
        (TestEntity->Type == ET_STAIRS)
    )
    {
        Result = true;
    }
    return Result;
}

internal void
HandleEntityOverlapWithStairs(game_state *GameState, entity *MovingEntity, entity *TestEntity, f32 TimeDelta, f32 *CurrentGroundLevel)
{
    if (TestEntity->Type == ET_STAIRS)
    {
        *CurrentGroundLevel = GetStairsGroundLevel(TestEntity, GetEntityGroundPoint(MovingEntity));
    }
}

internal b32
SpeculativeCollision(entity *MovingEntity, entity *TestEntity)
{
    b32 WillEntitiesCollide = true;
    if (TestEntity->Type == ET_STAIRS)
    {
        f32 StairStepHeight = 0.1f;

#if 0
        WillEntitiesCollide =
        (
            (AbsoluteValue(GetEntityGroundPoint(MovingEntity).Z - GroundLevel) > StairStepHeight) ||
            ((BarycentricMovingEntityPosition.Y > 0.1f) && (BarycentricMovingEntityPosition.Y < 0.9f))
        );
#endif

        v3 MovingEntityLowermostPoint = GetEntityGroundPoint(MovingEntity);
        f32 GroundLevel = GetStairsGroundLevel(TestEntity, MovingEntityLowermostPoint);
        WillEntitiesCollide = AbsoluteValue(MovingEntityLowermostPoint.Z - GroundLevel) > StairStepHeight;
    }
    return WillEntitiesCollide;
}

internal void
MoveEntity(game_state *GameState, simulation_region *SimulationRegion, entity *MovingEntity,
           v3 Acceleration, f32 TimeDelta, move_spec *MoveSpec)
{
    Assert(!IsFlagSet(MovingEntity, EF_NON_SPATIAL));

    if (MoveSpec->NormalizeAcceleration)
    {
        if (LengthSquared(Acceleration) > 1.0f)
        {
            Acceleration = Normalize(Acceleration);
        }
    }
    Acceleration.XY *= MoveSpec->SpeedInXYPlane;
    Acceleration.XY -= MoveSpec->DragInXYPlane * MovingEntity->Velocity.XY;

    if (!IsFlagSet(MovingEntity, EF_Z_SUPPORTED))
    {
        Acceleration.Z += -9.8f;
    }

    // TODO: improve velocity and postition update accuracy
    MovingEntity->Velocity = Acceleration * TimeDelta + MovingEntity->Velocity;
    Assert(Length(MovingEntity->Velocity) < SimulationRegion->MaxEntityVelocity);

    v3 MovementOffset =
        0.5f * Acceleration * Square(TimeDelta) +
        MovingEntity->Velocity * TimeDelta;
    
    if (MovingEntity->Position.Z < 0)
    {
        MovingEntity->Position.Z = 0;
    }

#define INIFINTE_MOVEMENT_DISTANCE 10000.0f
    f32 RemainingMovementDistance = MovingEntity->MovementDistanceLimit;
    if (RemainingMovementDistance == 0)
    {
        RemainingMovementDistance = INIFINTE_MOVEMENT_DISTANCE;
    }

    for (u32 CollisionDetectionIteration = 0; CollisionDetectionIteration < 4; CollisionDetectionIteration++)
    {
        f32 MovementDistance = Length(MovementOffset);
        if (MovementDistance > 0)
        {
            entity *HitEntity = 0;
            v3 CollisionNormal = V3(0, 0, 0);

            f32 TMin = 1.0f;
            if (RemainingMovementDistance < MovementDistance)
            {
                TMin = RemainingMovementDistance / MovementDistance;
            }

            v3 TargetPosition = MovingEntity->Position + MovementOffset;

            if (!IsFlagSet(MovingEntity, EF_NON_SPATIAL))
            {
                // TODO(marwan): reduce the number of tested against entities in the sim region
                //               using spatial partition
                entity *TestEntity = SimulationRegion->Entities;
                for
                (
                    u32 TestEntityIndex = 0;
                    TestEntityIndex < SimulationRegion->CurrentEntityCount;
                    TestEntityIndex++, TestEntity++
                )
                {
                    if (CanEntitiesCollide(GameState, TestEntity, MovingEntity))
                    {
                        b32 TestEntityWasHit = false;
                        v3 CollisionPointNormal = V3(0, 0, 0);
                        f32 TestTMin = TMin;

                        for 
                        (
                            u32 MovingEntityCollisionMeshIndex = 0; 
                            MovingEntityCollisionMeshIndex < MovingEntity->CollisionMeshGroup->MeshCount; 
                            MovingEntityCollisionMeshIndex++
                        )
                        {
                            entity_collision_mesh *MovingEntityCollisionMesh = &MovingEntity->CollisionMeshGroup->Meshes[MovingEntityCollisionMeshIndex];
                            for
                            (
                                u32 TestEntityCollisionMeshIndex = 0;
                                TestEntityCollisionMeshIndex < TestEntity->CollisionMeshGroup->MeshCount;
                                TestEntityCollisionMeshIndex++
                            )
                            {
                                entity_collision_mesh *TestEntityCollisionMesh = &TestEntity->CollisionMeshGroup->Meshes[TestEntityCollisionMeshIndex];

                                v3 MinkowskiCollisionDiameter = TestEntityCollisionMesh->Diameter + MovingEntityCollisionMesh->Diameter;

                                v3 CollisionAreaMinCorner = -0.5f * MinkowskiCollisionDiameter;
                                v3 CollisionAreaMaxCorner = CollisionAreaMinCorner + MinkowskiCollisionDiameter;

                                v3 TestMeshToMovingMesh =
                                    (MovingEntity->Position + MovingEntityCollisionMesh->Offset) - 
                                    (TestEntity->Position + TestEntityCollisionMesh->Offset);

                                b32 EntitiesOverlapInZ =
                                (
                                    (TestMeshToMovingMesh.Z >= CollisionAreaMinCorner.Z) &&
                                    (TestMeshToMovingMesh.Z < CollisionAreaMaxCorner.Z)
                                );

                                if (EntitiesOverlapInZ)
                                {
                                    if (TestWall(CollisionAreaMinCorner.X, TestMeshToMovingMesh.X, TestMeshToMovingMesh.Y,
                                                 MovementOffset.X, MovementOffset.Y,
                                                 &TestTMin, CollisionAreaMinCorner.Y, CollisionAreaMaxCorner.Y))
                                    {
                                        CollisionPointNormal = V3(-1, 0, 0);
                                        TestEntityWasHit = true;
                                    }

                                    if (TestWall(CollisionAreaMaxCorner.X, TestMeshToMovingMesh.X, TestMeshToMovingMesh.Y,
                                                 MovementOffset.X, MovementOffset.Y,
                                                 &TestTMin, CollisionAreaMinCorner.Y, CollisionAreaMaxCorner.Y))
                                    {
                                        CollisionPointNormal = V3(1, 0, 0);
                                        TestEntityWasHit = true;
                                    }

                                    if (TestWall(CollisionAreaMinCorner.Y, TestMeshToMovingMesh.Y, TestMeshToMovingMesh.X,
                                                 MovementOffset.Y, MovementOffset.X,
                                                 &TestTMin, CollisionAreaMinCorner.X, CollisionAreaMaxCorner.X))
                                    {
                                        CollisionPointNormal = V3(0, -1, 0);
                                        TestEntityWasHit = true;
                                    }

                                    if (TestWall(CollisionAreaMaxCorner.Y, TestMeshToMovingMesh.Y, TestMeshToMovingMesh.X,
                                                 MovementOffset.Y, MovementOffset.X,
                                                 &TestTMin, CollisionAreaMinCorner.X, CollisionAreaMaxCorner.X))
                                    {
                                        CollisionPointNormal = V3(0, 1, 0);
                                        TestEntityWasHit = true;
                                    }

                                    if (TestEntityWasHit)
                                    {
                                        if (SpeculativeCollision(MovingEntity, TestEntity))
                                        {
                                            HitEntity = TestEntity;
                                            TMin = TestTMin;
                                            CollisionNormal = CollisionPointNormal;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            MovingEntity->Position += TMin * MovementOffset;
            MovementOffset = TargetPosition - MovingEntity->Position;
            RemainingMovementDistance -= TMin * MovementDistance;

            if (HitEntity)
            {
                b32 EntityStoppedOnCollision = ProcessCollisionEvent(GameState, MovingEntity, HitEntity);
                if (EntityStoppedOnCollision)
                {
                    MovementOffset = MovementOffset - 1 * Inner(MovementOffset, CollisionNormal) * CollisionNormal;
                    MovingEntity->Velocity =
                        MovingEntity->Velocity - 1 * Inner(MovingEntity->Velocity, CollisionNormal) * CollisionNormal;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    f32 GroundLevel = 0;
    rectangle3 MovingEntityVolume = RectCenterDiameter
    (
        MovingEntity->Position + MovingEntity->CollisionMeshGroup->TotalCollisionMesh.Offset,
        MovingEntity->CollisionMeshGroup->TotalCollisionMesh.Diameter
    );

    entity *TestEntity = SimulationRegion->Entities;
    for
    (
        u32 TestEntityIndex = 0;
        TestEntityIndex < SimulationRegion->CurrentEntityCount;
        TestEntityIndex++, TestEntity++
    )
    {
        rectangle3 TestEntityVolume = RectCenterDiameter
        (
            TestEntity->Position + TestEntity->CollisionMeshGroup->TotalCollisionMesh.Offset,
            TestEntity->CollisionMeshGroup->TotalCollisionMesh.Diameter
        );

        if
        (
            CanEntitiesOverlap(GameState, MovingEntity, TestEntity) &&
            DoRectanglesOverlap(MovingEntityVolume, TestEntityVolume)
        )
        {
            HandleEntityOverlapWithStairs(GameState, MovingEntity, TestEntity, TimeDelta, &GroundLevel);
        }
    }

    f32 MovingEntityTargetZLevel = GroundLevel + (MovingEntity->Position.Z - GetEntityGroundPoint(MovingEntity).Z);
    if
    (
        (MovingEntity->Position.Z <= MovingEntityTargetZLevel) ||
        (
            IsFlagSet(MovingEntity, EF_Z_SUPPORTED) &&
            (MovingEntity->Velocity.Z == 0)
        )
    )
    {
        MovingEntity->Position.Z = MovingEntityTargetZLevel;
        MovingEntity->Velocity.Z = 0;
        SetFlags(MovingEntity, EF_Z_SUPPORTED);
    }
    else
    {
        ClearFlags(MovingEntity, EF_Z_SUPPORTED);
    }

    if (MovingEntity->MovementDistanceLimit != 0)
    {
        MovingEntity->MovementDistanceLimit = RemainingMovementDistance;
    }

    // TODO: change to using the Acceleration vector instead
    if ((MovingEntity->Velocity.X == 0) && (MovingEntity->Velocity.Y == 0))
    {
    }
    else if (AbsoluteValue(MovingEntity->Velocity.X) > AbsoluteValue(MovingEntity->Velocity.Y))
    {
        if (MovingEntity->Velocity.X > 0)
        {
            MovingEntity->BitmapFacingDirection = 0;
        }
        else
        {
            MovingEntity->BitmapFacingDirection = 2;
        }
    }
    else
    {
        if (MovingEntity->Velocity.Y > 0)
        {
            MovingEntity->BitmapFacingDirection = 1;
        }
        else
        {
            MovingEntity->BitmapFacingDirection = 3;
        }
    }
}
