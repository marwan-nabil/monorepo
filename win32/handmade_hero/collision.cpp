inline entity_collision_mesh_group *
MakeSimpleCollisionMeshTemplate(game_state *GameState, v3 CollisionMeshDiameter)
{
    entity_collision_mesh_group *Result = PushStruct(&GameState->WorldArena, entity_collision_mesh_group);

    Result->MeshCount = 1;
    Result->Meshes = PushArray(&GameState->WorldArena, Result->MeshCount, entity_collision_mesh);

    Result->TotalCollisionMesh.Diameter = CollisionMeshDiameter;
    Result->TotalCollisionMesh.Offset = V3(0, 0, 0.5f * CollisionMeshDiameter.Z);

    Result->Meshes[0].Diameter = Result->TotalCollisionMesh.Diameter;
    Result->Meshes[0].Offset = Result->TotalCollisionMesh.Offset;

    return Result;
}

inline entity_collision_mesh_group *
MakeNullCollisionMeshTemplate(game_state *GameState)
{
    entity_collision_mesh_group *Result = PushStruct(&GameState->WorldArena, entity_collision_mesh_group);
    Result->MeshCount = 0;
    Result->Meshes = 0;
    Result->TotalCollisionMesh.Diameter = V3(0, 0, 0);
    Result->TotalCollisionMesh.Offset = V3(0, 0, 0);
    return Result;
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

    u32 HashValue = FirstEntityStorageIndex & (ArrayCount(GameState->CollisionRulesTable) - 1);
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
ClearAllEntityCollisionRules(game_state *GameState, u32 StorageIndex)
{
    // TODO: improve collision rule storage data structure to optimize for insertion and clearing
    for (u32 HashValue = 0; HashValue < ArrayCount(GameState->CollisionRulesTable); HashValue++)
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

            u32 HashTableIndex = A->StorageIndex & (ArrayCount(GameState->CollisionRulesTable) - 1);
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
ProcessEntityCollision(game_state *GameState, entity *MovingEntity, entity *StaticEntity)
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