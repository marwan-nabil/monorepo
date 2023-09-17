inline b32
IsEntityFlagSet(entity *Entity, u32 Flag)
{
    b32 Result = ((Entity->Flags & Flag) != 0);
    return Result;
}

inline b32
AreAnyEntityFlagsSet(entity *Entity, u32 Flags)
{
    b32 Result = ((Entity->Flags & Flags) != 0);
    return Result;
}

inline void
SetEntityFlags(entity *Entity, u32 Flags)
{
    Entity->Flags |= Flags;
}

inline void
ClearEntityFlags(entity *Entity, u32 Flags)
{
    Entity->Flags &= ~Flags;
}

inline void
ClearAllEntityFlags(entity *Entity)
{
    Entity->Flags = 0;
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

static b32
ProcessCollisionEvent(game_state *GameState, entity *MovingEntity, entity *StaticEntity)
{
    b32 MovingEntityShouldStopOnCollision = FALSE;

    if (MovingEntity->Type == ET_SWORD)
    {
        AddEntityCollisionRule(GameState, MovingEntity->StorageIndex, StaticEntity->StorageIndex, FALSE);
        MovingEntityShouldStopOnCollision = FALSE;
    }
    else
    {
        MovingEntityShouldStopOnCollision = TRUE;
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

static b32
CanEntitiesCollide(game_state *GameState, entity *A, entity *B)
{
    b32 Result = FALSE;

    if (A != B)
    {
        SortEntityPointersByEntityTypes(&A, &B);

        if (IsEntityFlagSet(A, EF_COLLIDES) && IsEntityFlagSet(B, EF_COLLIDES))
        {
            if (!IsEntityFlagSet(A, EF_NON_SPATIAL) && !IsEntityFlagSet(B, EF_NON_SPATIAL))
            {
                Result = TRUE;
            }

            u32 HashTableIndex = A->StorageIndex & (ArrayLength(GameState->CollisionRulesTable) - 1);
            for
            (
                entity_collision_rule *CurrentRule = GameState->CollisionRulesTable[HashTableIndex];
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

static b32
CanEntitiesOverlap(game_state *GameState, entity *MovingEntity, entity *TestEntity)
{
    b32 Result = FALSE;
    if 
    (
        (MovingEntity != TestEntity) &&
        (TestEntity->Type == ET_STAIRS)
    )
    {
        Result = TRUE;
    }
    return Result;
}

static void
InitializeEntityHitPoints(storage_entity *StorageEntity, u32 HitpointsCount)
{
    Assert(HitpointsCount < ArrayLength(StorageEntity->Entity.HitPoints));

    StorageEntity->Entity.HitPointsMax = HitpointsCount;

    for (u32 HitPointIndex = 0; HitPointIndex < HitpointsCount; HitPointIndex++)
    {
        StorageEntity->Entity.HitPoints[HitPointIndex].FilledAmount = ENTITY_HIT_POINT_SUBCOUNT;
        StorageEntity->Entity.HitPoints[HitPointIndex].Flags = 0;
    }
}

inline entity_movement_parameters
DefaultEntityMovementParameters()
{
    entity_movement_parameters Result;
    Result.NormalizeAcceleration = FALSE;
    Result.SpeedInXYPlane = 1.0f;
    Result.DragInXYPlane = 0;
    return Result;
}

inline void
MakeEntityNonSpatial(entity *Entity)
{
    SetEntityFlags(Entity, EF_NON_SPATIAL);
    Entity->Position = INVALID_ENTITY_POSITION;
}

inline void
MakeEntitySpatial(entity *Entity, v3 InitialPosition, v3 InitialVelocity)
{
    ClearEntityFlags(Entity, EF_NON_SPATIAL);
    Entity->Position = InitialPosition;
    Entity->Velocity = InitialVelocity;
}

inline v3
GetEntityGroundPoint(entity *Entity)
{
    v3 GroundPoint = Entity->Position;
    return GroundPoint;
}

inline f32
GetStairsEntityGroundLevel(entity *StairsEntity, v3 MeasurementPoint)
{
    Assert(StairsEntity->Type == ET_STAIRS);
    rectangle2 StairsWalkableRegion = RectCenterDiameter(StairsEntity->Position.XY, StairsEntity->WalkableDiameter);
    v2 BarycentricMeasurementPosition = Clamp01(GetbarycentricPoint(StairsWalkableRegion, MeasurementPoint.XY));
    f32 GroundLevel = StairsEntity->Position.Z + BarycentricMeasurementPosition.Y * StairsEntity->WalkableHeight;
    return GroundLevel;
}

static void
HandleEntityOverlapWithStairs(game_state *GameState, entity *MovingEntity, entity *TestEntity, f32 TimeDelta, f32 *CurrentGroundLevel)
{
    if (TestEntity->Type == ET_STAIRS)
    {
        *CurrentGroundLevel = GetStairsEntityGroundLevel(TestEntity, GetEntityGroundPoint(MovingEntity));
    }
}


static b32
SpeculativeCollision(entity *MovingEntity, entity *TestEntity)
{
    b32 WillEntitiesCollide = TRUE;
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
        f32 GroundLevel = GetStairsEntityGroundLevel(TestEntity, MovingEntityLowermostPoint);
        WillEntitiesCollide = AbsoluteValue(MovingEntityLowermostPoint.Z - GroundLevel) > StairStepHeight;
    }
    return WillEntitiesCollide;
}

inline storage_entity *
GetStorageEntity(game_state *GameState, u32 StorageIndex)
{
    storage_entity *Result = 0;

    if ((StorageIndex > 0) && (StorageIndex < GameState->World->StorageEntityCount))
    {
        Result = GameState->World->StorageEntities + StorageIndex;
    }

    return Result;
}

static add_storage_entity_result
AddStorageEntity(game_state *GameState, entity_type Type, world_position WorldPosition)
{
    Assert(GameState->World->StorageEntityCount < ArrayLength(GameState->World->StorageEntities));

    add_storage_entity_result Result;

    Result.StorageIndex = GameState->World->StorageEntityCount++;
    Result.StorageEntity = GameState->World->StorageEntities + Result.StorageIndex;

    *Result.StorageEntity = {};
    Result.StorageEntity->Entity.Type = Type;
    Result.StorageEntity->Entity.CollisionMeshGroup = GameState->NullCollisionMeshGroupTemplate;
    Result.StorageEntity->WorldPosition = InvalidWorldPosition();

    ChangeStorageEntityLocationInWorld(GameState->World, &GameState->WorldArena, Result.StorageIndex, Result.StorageEntity, &WorldPosition);

    return Result;
}

static add_storage_entity_result
AddGoundBasedStorageEntity
(
    game_state *GameState, entity_type Type, world_position GroundPoint,
    entity_collision_mesh_group *EntityCollisionMeshGroup
)
{
    // TODO(marwan): fix buggy Z handling here?
    add_storage_entity_result Result = AddStorageEntity(GameState, Type, GroundPoint);
    Result.StorageEntity->Entity.CollisionMeshGroup = EntityCollisionMeshGroup;
    return Result;
}

static add_storage_entity_result
AddStandardRoom(game_state *GameState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position RoomPosition = GetWorldPositionFromTilePosition(GameState->World, AbsTileX, AbsTileY, AbsTileZ, V3(0, 0, 0));

    add_storage_entity_result RoomStorageEntityResult =
        AddGoundBasedStorageEntity(GameState, ET_SPACE, RoomPosition, GameState->StandardRoomCollisionMeshGroupTemplate);

    SetEntityFlags(&RoomStorageEntityResult.StorageEntity->Entity, EF_TRAVERSABLE);

    return RoomStorageEntityResult;
}


static add_storage_entity_result
AddSword(game_state *GameState)
{
    add_storage_entity_result SwordStorageEntityResult = AddStorageEntity(GameState, ET_SWORD, InvalidWorldPosition());

    SwordStorageEntityResult.StorageEntity->Entity.CollisionMeshGroup = GameState->SwordCollisionMeshGroupTemplate;

    SetEntityFlags(&SwordStorageEntityResult.StorageEntity->Entity, EF_NON_SPATIAL | EF_MOVEABLE);
    
    return SwordStorageEntityResult;
}

static add_storage_entity_result
AddPlayer(game_state *GameState)
{
    add_storage_entity_result PlayerStorageEntityResult = 
        AddGoundBasedStorageEntity(GameState, ET_HERO, GameState->CameraPosition, GameState->PlayerCollisionMeshGroupTemplate);

    SetEntityFlags(&PlayerStorageEntityResult.StorageEntity->Entity, EF_COLLIDES | EF_MOVEABLE);
    InitializeEntityHitPoints(PlayerStorageEntityResult.StorageEntity, 3);

    add_storage_entity_result SwordStorageEntityResult = AddSword(GameState);
    PlayerStorageEntityResult.StorageEntity->Entity.SwordEntityReference.StorageIndex = SwordStorageEntityResult.StorageIndex;

    if (GameState->StorageIndexOfEntityThatCameraFollows == 0)
    {
        GameState->StorageIndexOfEntityThatCameraFollows = PlayerStorageEntityResult.StorageIndex;
    }

    return PlayerStorageEntityResult;
}

static add_storage_entity_result
AddMonster(game_state *GameState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position MonsterPosition = GetWorldPositionFromTilePosition(GameState->World, AbsTileX, AbsTileY, AbsTileZ, V3(0, 0, 0));

    add_storage_entity_result MonsterStorageEntityResult = 
        AddGoundBasedStorageEntity(GameState, ET_MONSTER, MonsterPosition, GameState->MonsterCollisionMeshGroupTemplate);

    SetEntityFlags(&MonsterStorageEntityResult.StorageEntity->Entity, EF_COLLIDES | EF_MOVEABLE);
    InitializeEntityHitPoints(MonsterStorageEntityResult.StorageEntity, 3);

    return MonsterStorageEntityResult;
}

static add_storage_entity_result
AddFamiliar(game_state *GameState, i32 AbsTileX, i32 AbsTileY, i32 AbsTileZ)
{
    world_position FamiliarPosition = GetWorldPositionFromTilePosition(GameState->World, AbsTileX, AbsTileY, AbsTileZ, V3(0, 0, 0));

    add_storage_entity_result FamiliarStorageEntityResult = 
        AddGoundBasedStorageEntity(GameState, ET_FAMILIAR, FamiliarPosition, GameState->FamiliarCollisionMeshGroupTemplate);

    SetEntityFlags(&FamiliarStorageEntityResult.StorageEntity->Entity, EF_COLLIDES | EF_MOVEABLE);

    return FamiliarStorageEntityResult;
}

static add_storage_entity_result
AddWall(game_state *GameState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position WallGroundPosition =
        GetWorldPositionFromTilePosition(GameState->World, AbsTileX, AbsTileY, AbsTileZ, V3(0, 0, 0));

    add_storage_entity_result WallStorageEntityResult = 
        AddGoundBasedStorageEntity(GameState, ET_WALL, WallGroundPosition, GameState->WallCollisionMeshGroupTemplate);

    SetEntityFlags(&WallStorageEntityResult.StorageEntity->Entity, EF_COLLIDES);

    return WallStorageEntityResult;
}

static add_storage_entity_result
AddStairs(game_state *GameState, u32 AbsTileX, u32 AbsTileY, u32 AbsTileZ)
{
    world_position StairsGroundPosition = GetWorldPositionFromTilePosition
    (
        GameState->World, AbsTileX, AbsTileY, AbsTileZ,
        V3(0, 0, 0)
    );

    // NOTE(marwan): this is the correct thing
    //world_position StairsGroundPosition = GetWorldPositionFromTilePosition
    //(
    //    GameState->World, AbsTileX, AbsTileY, AbsTileZ,
    //    V3(0, 0, -0.5f * StairDiameter.Z)
    //);

    add_storage_entity_result StairStorageEntityResult =
        AddGoundBasedStorageEntity(GameState, ET_STAIRS, StairsGroundPosition, GameState->StairsCollisionMeshGroupTemplate);

    SetEntityFlags(&StairStorageEntityResult.StorageEntity->Entity, EF_COLLIDES);

    StairStorageEntityResult.StorageEntity->Entity.WalkableHeight = GameState->World->TileDepthInMeters;
    StairStorageEntityResult.StorageEntity->Entity.WalkableDiameter =
        StairStorageEntityResult.StorageEntity->Entity.CollisionMeshGroup->TotalCollisionMesh.Diameter.XY;

    return StairStorageEntityResult;
}

static void
AddEntityCollisionRule(game_state *GameState, u32 FirstEntityStorageIndex, u32 SecondEntityStorageIndex, b32 CanCollide)
{
    entity_collision_rule *ResultRule = 0;

    if (FirstEntityStorageIndex > SecondEntityStorageIndex)
    {
        u32 SwappingTemporaryStorageIndex = FirstEntityStorageIndex;
        FirstEntityStorageIndex = SecondEntityStorageIndex;
        SecondEntityStorageIndex = SwappingTemporaryStorageIndex;
    }

    u32 HashValue = FirstEntityStorageIndex & (ArrayLength(GameState->CollisionRulesTable) - 1);
    for
    (
        entity_collision_rule *CurrentRule = GameState->CollisionRulesTable[HashValue];
        CurrentRule;
        CurrentRule = CurrentRule->Next
    )
    {
        if ((CurrentRule->EntityAStorageIndex == FirstEntityStorageIndex) &&
            (CurrentRule->EntityBStorageIndex == SecondEntityStorageIndex))
        {
            ResultRule = CurrentRule;
            break;
        }
    }

    if (!ResultRule)
    {
        if (!GameState->FreeCollisionRulesListHead)
        {
            ResultRule = PushStruct(&GameState->WorldArena, entity_collision_rule);
        }
        else
        {
            ResultRule = GameState->FreeCollisionRulesListHead;
            GameState->FreeCollisionRulesListHead = GameState->FreeCollisionRulesListHead->Next;
        }

        ResultRule->Next = GameState->CollisionRulesTable[HashValue];
        GameState->CollisionRulesTable[HashValue] = ResultRule;
    }

    Assert(ResultRule);
    if (ResultRule)
    {
        ResultRule->EntityAStorageIndex = FirstEntityStorageIndex;
        ResultRule->EntityBStorageIndex = SecondEntityStorageIndex;
        ResultRule->CanCollide = CanCollide;
    }
}

static b32
RemoveEntityCollisionRule(game_state *GameState, u32 FirstEntityStorageIndex, u32 SecondEntityStorageIndex)
{
}

static void
ClearAllEnrityCollisionRules(game_state *GameState, u32 StorageIndex)
{
    // TODO: improve collision rule storage data structure to optimize for insertion and clearing
    for (u32 HashValue = 0; HashValue < ArrayLength(GameState->CollisionRulesTable); HashValue++)
    {
        for
        (
            entity_collision_rule **CurrentRulePointer = &GameState->CollisionRulesTable[HashValue];
            *CurrentRulePointer;
        )
        {
            if (((*CurrentRulePointer)->EntityAStorageIndex == StorageIndex) ||
                ((*CurrentRulePointer)->EntityBStorageIndex == StorageIndex))
            {
                entity_collision_rule *RuleToRemove = *CurrentRulePointer;
                
                *CurrentRulePointer = (*CurrentRulePointer)->Next;

                RuleToRemove->Next = GameState->FreeCollisionRulesListHead;
                GameState->FreeCollisionRulesListHead = RuleToRemove;
            }
            else
            {
                CurrentRulePointer = &(*CurrentRulePointer)->Next;
            }
        }
    }
}

static void
MoveEntity
(
    game_state *GameState, simulation_region *SimulationRegion, entity *MovingEntity,
    v3 Acceleration, f32 TimeDelta, entity_movement_parameters *MoveSpec
)
{
    Assert(!IsEntityFlagSet(MovingEntity, EF_NON_SPATIAL));

    if (MoveSpec->NormalizeAcceleration)
    {
        if (LengthSquared(Acceleration) > 1.0f)
        {
            Acceleration = Normalize(Acceleration);
        }
    }
    Acceleration.XY *= MoveSpec->SpeedInXYPlane;
    Acceleration.XY -= MoveSpec->DragInXYPlane * MovingEntity->Velocity.XY;

    if (!IsEntityFlagSet(MovingEntity, EF_Z_SUPPORTED))
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

            if (!IsEntityFlagSet(MovingEntity, EF_NON_SPATIAL))
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
                        b32 TestEntityWasHit = FALSE;
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
                                        TestEntityWasHit = TRUE;
                                    }

                                    if (TestWall(CollisionAreaMaxCorner.X, TestMeshToMovingMesh.X, TestMeshToMovingMesh.Y,
                                                 MovementOffset.X, MovementOffset.Y,
                                                 &TestTMin, CollisionAreaMinCorner.Y, CollisionAreaMaxCorner.Y))
                                    {
                                        CollisionPointNormal = V3(1, 0, 0);
                                        TestEntityWasHit = TRUE;
                                    }

                                    if (TestWall(CollisionAreaMinCorner.Y, TestMeshToMovingMesh.Y, TestMeshToMovingMesh.X,
                                                 MovementOffset.Y, MovementOffset.X,
                                                 &TestTMin, CollisionAreaMinCorner.X, CollisionAreaMaxCorner.X))
                                    {
                                        CollisionPointNormal = V3(0, -1, 0);
                                        TestEntityWasHit = TRUE;
                                    }

                                    if (TestWall(CollisionAreaMaxCorner.Y, TestMeshToMovingMesh.Y, TestMeshToMovingMesh.X,
                                                 MovementOffset.Y, MovementOffset.X,
                                                 &TestTMin, CollisionAreaMinCorner.X, CollisionAreaMaxCorner.X))
                                    {
                                        CollisionPointNormal = V3(0, 1, 0);
                                        TestEntityWasHit = TRUE;
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
            IsEntityFlagSet(MovingEntity, EF_Z_SUPPORTED) &&
            (MovingEntity->Velocity.Z == 0)
        )
    )
    {
        MovingEntity->Position.Z = MovingEntityTargetZLevel;
        MovingEntity->Velocity.Z = 0;
        SetEntityFlags(MovingEntity, EF_Z_SUPPORTED);
    }
    else
    {
        ClearEntityFlags(MovingEntity, EF_Z_SUPPORTED);
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