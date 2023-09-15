#include "handmade_platform.h"
#include "handmade_math.h"
#include "handmade_simulation_region.h"
#include "handmade_entity.h"

inline move_spec
DefaultMoveSpec()
{
    move_spec Result;
    Result.NormalizeAcceleration = false;
    Result.SpeedInXYPlane = 1.0f;
    Result.DragInXYPlane = 0;
    return Result;
}

inline void
MakeEntityNonSpatial(entity *Entity)
{
    SetFlags(Entity, EF_NON_SPATIAL);
    Entity->Position = INVALID_ENTITY_POSITION;
}

inline void
MakeEntitySpatial(entity *Entity, v3 InitialPosition, v3 InitialVelocity)
{
    ClearFlags(Entity, EF_NON_SPATIAL);
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
GetStairsGroundLevel(entity *StairsEntity, v3 MeasurementPoint)
{
    Assert(StairsEntity->Type == ET_STAIRS);

    rectangle2 StairsWalkableRegion = RectCenterDiameter(StairsEntity->Position.XY, StairsEntity->WalkableDiameter);

    v2 BarycentricMeasurementPosition = Clamp01(GetbarycentricPoint(StairsWalkableRegion, MeasurementPoint.XY));

    f32 GroundLevel = StairsEntity->Position.Z + BarycentricMeasurementPosition.Y * StairsEntity->WalkableHeight;
    return GroundLevel;
}
