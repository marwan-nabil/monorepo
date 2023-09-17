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