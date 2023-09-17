#include <stdint.h>
#include <math.h>
#include <intrin.h>

#include "../../miscellaneous/base_types.h"
#include "../../miscellaneous/assertions.h"
#include "../../miscellaneous/basic_defines.h"

#include "../../math/constants.h"
#include "../../math/vector2.h"
#include "../../math/vector3.h"
#include "../../math/vector4.h"
#include "../../math/vector4.h"
#include "../../math/rectangle2.h"
#include "../../math/rectangle3.h"
#include "../../math/bit_operations.h"

#include "game_interface.h"

// =====================================================
#include "../../math/floats.cpp"
#include "../../math/integers.cpp"
#include "../../math/scalar_conversions.cpp"
#include "../../math/transcendentals.cpp"
#include "../../math/bit_operations.cpp"

#include "game_interface.cpp"

inline void
InitializeMemoryArena(memory_arena *Arena, size_t Size, void *Base)
{
    Arena->Size = Size;
    Arena->Used = 0;
    Arena->Base = (u8 *)Base;
}

inline void *
PushOntoMemoryArena(memory_arena *Arena, size_t PushSize)
{
    Assert((Arena->Used + PushSize) <= Arena->Size);
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += PushSize;
    return Result;
}

static void
DrawRectangle
(
    game_pixel_buffer *PixelBuffer,
    v2 MinCorner, v2 MaxCorner, 
    f32 Red, f32 Green, f32 Blue, f32 Alpha
)
{
    i32 MinX = RoundF32ToI32(MinCorner.X);
    i32 MinY = RoundF32ToI32(MinCorner.Y);
    i32 MaxX = RoundF32ToI32(MaxCorner.X);
    i32 MaxY = RoundF32ToI32(MaxCorner.Y);

    if (MinX < 0)
    {
        MinX = 0;
    }
    if (MinY < 0)
    {
        MinY = 0;
    }
    if (MaxX > PixelBuffer->WidthInPixels)
    {
        MaxX = PixelBuffer->WidthInPixels;
    }
    if (MaxY > PixelBuffer->HeightInPixels)
    {
        MaxY = PixelBuffer->HeightInPixels;
    }

    u32 Color = (u32)
    (
        RoundF32ToU32(Alpha * 255.0f) << 24 |
        RoundF32ToU32(Red * 255.0f) << 16 |
        RoundF32ToU32(Green * 255.0f) << 8 |
        RoundF32ToU32(Blue * 255.0f)
    );

    u8 *Row = ((u8 *)PixelBuffer->PixelsMemory + MinX * PixelBuffer->BytesPerPixel + MinY * PixelBuffer->BytesPerRow);

    for (i32 Y = MinY; Y < MaxY; Y++)
    {
        u32 *Pixel = (u32 *)Row;

        for (i32 X = MinX; X < MaxX; X++)
        {
            f32 SourceAlpha = (f32)((Color >> 24) & 0xff) / 255.0f;
            f32 SourceRed = (f32)((Color >> 16) & 0xff);
            f32 SourceGreen = (f32)((Color >> 8) & 0xff);
            f32 SourceBlue = (f32)((Color >> 0) & 0xff);

            f32 DestinationRed = (f32)((*Pixel >> 16) & 0xff);
            f32 DestinationGreen = (f32)((*Pixel >> 8) & 0xff);
            f32 DestinationBlue = (f32)((*Pixel >> 0) & 0xff);

            f32 ResultRed = SourceAlpha * SourceRed + (1 - SourceAlpha) * DestinationRed;
            f32 ResultGreen = SourceAlpha * SourceGreen + (1 - SourceAlpha) * DestinationGreen;
            f32 ResultBlue = SourceAlpha * SourceBlue + (1 - SourceAlpha) * DestinationBlue;

            *Pixel++ =
                ((u32)(ResultRed + 0.5f) << 16) |
                ((u32)(ResultGreen + 0.5f) << 8) |
                ((u32)(ResultBlue + 0.5f) << 0);
        }

        Row += PixelBuffer->BytesPerRow;
    }
}

#pragma pack(push, 1)
struct bitmap_header
{
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;
    u32 Size;
    i32 Width;
    i32 Height;
    u16 Planes;
    u16 BitsPerPixel;
    u32 Compression;
    u32 SizeOfBitMap;
    i32 HorizontalResolution;
    i32 VerticalResolution;
    u32 ColorsUsed;
    u32 ColorsImportant;
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
};
#pragma pack(pop)

static loaded_bitmap
LoadBitmap(platform_read_file *ReadEntireFile, thread_context *ThreadContext, char *FileName)
{
    loaded_bitmap Result = {};

    read_file_result ReadResult = ReadEntireFile(ThreadContext, FileName);
    if (ReadResult.ConstentsSize)
    {
        bitmap_header *Header = (bitmap_header *)ReadResult.FileContents;
        Assert(Header->Compression == 3);

        u32 SourceRedMask = Header->RedMask;
        u32 SourceGreenMask = Header->GreenMask;
        u32 SourceBlueMask = Header->BlueMask;
        u32 SourceAlphaMask = ~(SourceRedMask | SourceGreenMask | SourceBlueMask);

        bitscan_result BitscanResult = FindLeastSignificantSetBit(SourceRedMask);
        Assert(BitscanResult.Found);
        u32 SourceRedShift = BitscanResult.Index;
        
        BitscanResult = FindLeastSignificantSetBit(SourceGreenMask);
        Assert(BitscanResult.Found);
        u32 SourceGreenShift = BitscanResult.Index;
        
        BitscanResult = FindLeastSignificantSetBit(SourceBlueMask);
        Assert(BitscanResult.Found);
        u32 SourceBlueShift = BitscanResult.Index;
        
        BitscanResult = FindLeastSignificantSetBit(SourceAlphaMask);
        Assert(BitscanResult.Found);
        u32 SourceAlphaShift = BitscanResult.Index;
        
        // calculate needed bit rotations to the left
        i32 AlphaTargetLeftRotation = 24 - (i32)SourceAlphaShift;
        i32 RedTargetLeftRotation = 16 - (i32)SourceRedShift;
        i32 GreenTargetLeftRotation = 8 - (i32)SourceGreenShift;
        i32 BlueTargetLeftRotation = 0 - (i32)SourceBlueShift;

        Result.Pixels = (u32 *)((u8 *)ReadResult.FileContents + Header->BitmapOffset);
  
        // NOTE: target pixel format 0x AA RR GG BB
        u32 *SourceAndDest = Result.Pixels;
        for (i32 Y = 0; Y < Header->Height; Y++)
        {
            for (i32 X = 0; X < Header->Width; X++)
            {
                u32 Value = *SourceAndDest;
                *SourceAndDest++ =
                    RotateLeft(Value & SourceAlphaMask, AlphaTargetLeftRotation) |
                    RotateLeft(Value & SourceRedMask, RedTargetLeftRotation) |
                    RotateLeft(Value & SourceGreenMask, GreenTargetLeftRotation) |
                    RotateLeft(Value & SourceBlueMask, BlueTargetLeftRotation);
            }
        }

        Result.Width = Header->Width;
        Result.Height = Header->Height;
    }
    return Result;
}

static void
GameOutputSound(game_state *GameState, game_sound_request *SoundRequest, u32 Frequency)
{
    i16 ToneVolume = 200;
    u32 SamplesPerCycle = SoundRequest->SamplesPerSecond / Frequency;

    i16 *SampleOut = SoundRequest->SamplesMemory;

    for
    (
        u32 SampleIndex = 0;
        SampleIndex < SoundRequest->OutputSamplesCount;
        SampleIndex++
    )
    {
#if 0
        f32 SinValue = Sin(GameState->SinParameterInRadians);

        i16 SampleValue = (i16)(ToneVolume * SinValue);

        GameState->SinParameterInRadians += 1.0f * 2.0f * PI32 / (f32)SamplesPerCycle;
        if (GameState->SinParameterInRadians > 2.0f * PI32)
        {
            GameState->SinParameterInRadians -= 2.0f * PI32;
        }
#else
        i16 SampleValue = 0;
#endif
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;
    }
}

static void
DrawBitmap
(
    loaded_bitmap *SourceBitMap, game_pixel_buffer *DestinationBuffer, 
    f32 DestinationX, f32 DestinationY, f32 AlphaFactor
)
{
    i32 DestMinX = RoundF32ToI32(DestinationX);
    i32 DestMinY = RoundF32ToI32(DestinationY);
    i32 DestMaxX = DestMinX + SourceBitMap->Width;
    i32 DestMaxY = DestMinY + SourceBitMap->Height;

    i32 SourceOffsetX = 0;
    i32 SourceOffsetY = 0;

    if (DestMinX < 0)
    {
        SourceOffsetX = -DestMinX;
        DestMinX = 0;
    }
    if (DestMinY < 0)
    {
        SourceOffsetY = -DestMinY;
        DestMinY = 0;
    }
    if (DestMaxX > DestinationBuffer->WidthInPixels)
    {
        DestMaxX = DestinationBuffer->WidthInPixels;
    }
    if (DestMaxY > DestinationBuffer->HeightInPixels)
    {
        DestMaxY = DestinationBuffer->HeightInPixels;
    }

    u32 *SourcePixelsRow =
        SourceBitMap->Pixels +
        SourceOffsetX +
        SourceBitMap->Width * (SourceBitMap->Height - 1 - SourceOffsetY);

    u8 *DestinationRow = 
        (u8 *)DestinationBuffer->PixelsMemory + 
        DestMinX * DestinationBuffer->BytesPerPixel + 
        DestMinY * DestinationBuffer->BytesPerRow;

    for (i32 Y = DestMinY; Y < DestMaxY; Y++)
    {
        u32 *Destination = (u32 *)DestinationRow;
        u32 *Source = SourcePixelsRow;

        for (i32 X = DestMinX; X < DestMaxX; X++)
        {
            f32 SourceAlpha = (f32)((*Source >> 24) & 0xff) / 255.0f * AlphaFactor;
            f32 SourceRed = (f32)((*Source >> 16) & 0xff);
            f32 SourceGreen = (f32)((*Source >> 8) & 0xff);
            f32 SourceBlue = (f32)((*Source >> 0) & 0xff);

            f32 DestinationRed = (f32)((*Destination >> 16) & 0xff);
            f32 DestinationGreen = (f32)((*Destination >> 8) & 0xff);
            f32 DestinationBlue = (f32)((*Destination >> 0) & 0xff);

            f32 ResultRed = SourceAlpha * SourceRed + (1 - SourceAlpha) * DestinationRed;
            f32 ResultGreen = SourceAlpha * SourceGreen + (1 - SourceAlpha) * DestinationGreen;
            f32 ResultBlue = SourceAlpha * SourceBlue + (1 - SourceAlpha) * DestinationBlue;

            *Destination =
                ((u32)(ResultRed + 0.5f) << 16) |
                ((u32)(ResultGreen + 0.5f) << 8) |
                ((u32)(ResultBlue + 0.5f) << 0);

            Destination++;
            Source++;
        }

        DestinationRow += DestinationBuffer->BytesPerRow;
        SourcePixelsRow -= SourceBitMap->Width;
    }
}

inline void
PushRenderPeice
(
    game_state *GameState, render_peice_group *PeiceGroup, 
    loaded_bitmap *Bitmap, v3 Offset, v2 BitmapAlignment,
    v4 Color, f32 EntityJumpZCoefficient, v2 RectangleDimensions
)
{
    Assert(PeiceGroup->Count < ArrayLength(PeiceGroup->Peices));

    render_piece *NewRenderPiece = PeiceGroup->Peices + PeiceGroup->Count++;
    NewRenderPiece->Bitmap = Bitmap;
    NewRenderPiece->Offset.XY = V2(Offset.X, -Offset.Y) * GameState->PixelsToMetersRatio - BitmapAlignment;
    NewRenderPiece->Offset.Z = Offset.Z;
    NewRenderPiece->EntityJumpZCoefficient = EntityJumpZCoefficient;
    NewRenderPiece->Dimensions = RectangleDimensions;
    NewRenderPiece->Color = Color;
}

inline void
PushBitmapRenderPiece
(
    game_state *GameState, render_peice_group *PeiceGroup, 
    loaded_bitmap *Bitmap, v3 Offset, v2 BitmapAlignment,
    f32 Alpha, f32 EntityJumpZCoefficient
)
{
    PushRenderPeice(GameState, PeiceGroup, Bitmap, Offset, BitmapAlignment, V4(0, 0, 0, Alpha), EntityJumpZCoefficient, V2(0, 0));
}

inline void
PushRectangleRenderPiece
(
    game_state *GameState, render_peice_group *PeiceGroup, v3 Offset,
    v2 RectangleDimensions, v4 Color, f32 EntityJumpZCoefficient
)
{
    PushRenderPeice(GameState, PeiceGroup, 0, Offset, V2(0, 0), Color, EntityJumpZCoefficient, RectangleDimensions);
}

inline void
PushRectangleOutlineRenderPieces
(
    game_state *GameState, render_peice_group *PeiceGroup,
    v3 Offset, v2 RectangleDimensions, v4 Color, f32 EntityJumpZCoefficient
)
{
    f32 Thickness = 0.1f;
    PushRenderPeice
    (
        GameState, PeiceGroup, 0, Offset - V3(0, RectangleDimensions.Y / 2.0f, 0),
        V2(0, 0), Color, EntityJumpZCoefficient, V2(RectangleDimensions.X, Thickness)
    );
    PushRenderPeice
    (
        GameState, PeiceGroup, 0, Offset + V3(0, RectangleDimensions.Y / 2.0f, 0),
        V2(0, 0), Color, EntityJumpZCoefficient, V2(RectangleDimensions.X, Thickness)
    );
    PushRenderPeice
    (
        GameState, PeiceGroup, 0, Offset - V3(RectangleDimensions.X / 2.0f, 0, 0),
        V2(0, 0), Color, EntityJumpZCoefficient, V2(Thickness, RectangleDimensions.Y)
    );
    PushRenderPeice
    (
        GameState, PeiceGroup, 0, Offset + V3(RectangleDimensions.X / 2.0f, 0, 0),
        V2(0, 0), Color, EntityJumpZCoefficient, V2(Thickness, RectangleDimensions.Y)
    );
}

inline void
DrawHitpoints(game_state *GameState, render_peice_group *EntityPeiceGroup, entity *Entity)
{
    if (Entity->HitPointsMax >= 1)
    {
        v2 HitPointDimension = V2(0.2f, 0.2f);
        f32 HitPointXSpacing = 1.5f * HitPointDimension.X;
        v3 HitPointDelta = V3(HitPointXSpacing, 0, 0);
        v3 HitPointOffset = V3(-0.5f * HitPointXSpacing * (Entity->HitPointsMax - 1), -0.25f, 0);

        for (u32 HitPointIndex = 0; HitPointIndex < Entity->HitPointsMax; HitPointIndex++)
        {
            entity_hit_point *HitPoint = Entity->HitPoints + HitPointIndex;
            v4 Color = V4(1.0f, 0, 0, 1.0f);
            if (HitPoint->FilledAmount == 0)
            {
                Color = V4(0.2f, 0.2f, 0.2f, 1.0f);
            }

            PushRectangleRenderPiece
            (
                GameState, EntityPeiceGroup, HitPointOffset, HitPointDimension, Color, 0.0f
            );
            HitPointOffset += HitPointDelta;
        }
    }
}

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

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
//void GameUpdateAndRender(thread_context *ThreadContext, game_pixel_buffer *PixelBuffer, game_input *GameInput, game_memory *GameMemory)
{
    Assert
    (
        (&GameInput->ControllerStates[0].Start - &GameInput->ControllerStates[0].Buttons[0])
        ==
        (ArrayLength(GameInput->ControllerStates[0].Buttons) - 1)
    );
    Assert(sizeof(game_state) <= GameMemory->PermanentStorageSize);

    game_state *GameState = (game_state *)GameMemory->PermanentStorage;

    if (!GameMemory->IsInitialized)
    {
        GameMemory->IsInitialized = TRUE;

        InitializeMemoryArena
        (
            &GameState->WorldArena,
            GameMemory->PermanentStorageSize - sizeof(game_state),
            (void *)((size_t)GameMemory->PermanentStorage + sizeof(game_state))
        );

        u32 TilesPerScreenWidth = 17;
        u32 TilesPerScreenHeight = 9;
        f32 TileSideInMeters = 1.4f;
        f32 TileDepthInMeters = 3.0f;
        i32 TileSideInPixels = 60;

        GameState->NullCollisionMeshGroupTemplate = 
            MakeNullCollisionMeshTemplate(GameState);
        GameState->SwordCollisionMeshGroupTemplate = 
            MakeSimpleCollisionMeshTemplate(GameState, V3(1.0f, 0.5f, 0.1f));
        GameState->WallCollisionMeshGroupTemplate = 
            MakeSimpleCollisionMeshTemplate(GameState, V3(TileSideInMeters, TileSideInMeters, TileDepthInMeters));
        GameState->StandardRoomCollisionMeshGroupTemplate = 
            MakeSimpleCollisionMeshTemplate(GameState, V3(TilesPerScreenWidth * TileSideInMeters, TilesPerScreenHeight * TileSideInMeters, 0.9f * TileDepthInMeters));
        GameState->StairsCollisionMeshGroupTemplate = 
            MakeSimpleCollisionMeshTemplate(GameState, V3(TileSideInMeters, 2 * TileSideInMeters, 1.1f * TileDepthInMeters));
        GameState->PlayerCollisionMeshGroupTemplate = 
            MakeSimpleCollisionMeshTemplate(GameState, V3(1.0f, 0.5f, 1.2f));
        GameState->FamiliarCollisionMeshGroupTemplate = 
            MakeSimpleCollisionMeshTemplate(GameState, V3(1.0f, 0.5f, 0.5f));
        GameState->MonsterCollisionMeshGroupTemplate = 
            MakeSimpleCollisionMeshTemplate(GameState, V3(1.0f, 0.5f, 0.5f));

        GameState->BackDropBitMap = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/../data/test/test_background.bmp");
        GameState->ShadowBitMap = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/../data/test/test_hero_shadow.bmp");
        GameState->TreeBitMap = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test2/tree00.bmp");
        GameState->StairWellBitMap = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test2/rock02.bmp");
        GameState->SwordBitMap = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test2/rock03.bmp");

        hero_bitmap_group *HeroBitmapGroup = &GameState->HeroBitmapGroups[0];
        HeroBitmapGroup->Head = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_right_head.bmp");
        HeroBitmapGroup->Cape = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_right_cape.bmp");
        HeroBitmapGroup->Torso = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_right_torso.bmp");
        HeroBitmapGroup->Alignment = V2(72, 182);

        HeroBitmapGroup = &GameState->HeroBitmapGroups[1];
        HeroBitmapGroup->Head = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_back_head.bmp");
        HeroBitmapGroup->Cape = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_back_cape.bmp");
        HeroBitmapGroup->Torso = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_back_torso.bmp");
        HeroBitmapGroup->Alignment = V2(72, 182);

        HeroBitmapGroup = &GameState->HeroBitmapGroups[2];
        HeroBitmapGroup->Head = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_left_head.bmp");
        HeroBitmapGroup->Cape = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_left_cape.bmp");
        HeroBitmapGroup->Torso = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_left_torso.bmp");
        HeroBitmapGroup->Alignment = V2(72, 182);

        HeroBitmapGroup = &GameState->HeroBitmapGroups[3];
        HeroBitmapGroup->Head = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_front_head.bmp");
        HeroBitmapGroup->Cape = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_front_cape.bmp");
        HeroBitmapGroup->Torso = 
            LoadBitmap(GameMemory->PlatformReadFile, ThreadContext, (char *)"../data/test/test_hero_front_torso.bmp");
        HeroBitmapGroup->Alignment = V2(72, 182);

        world *World = PushStruct(&GameState->WorldArena, world);
        GameState->World = World;
        InitializeWorld(World, TileSideInMeters, TileDepthInMeters);

        GameState->PixelsToMetersRatio = (f32)TileSideInPixels / TileSideInMeters;
        
        AddStorageEntity(GameState, ET_NULL, InvalidWorldPosition());

        u32 RandomNumbersIndex = 99;

        u32 StartScreenX = 0;// INT16_MAX / TilesPerScreenWidth / 2;
        u32 StartScreenY = 0;//INT16_MAX / TilesPerScreenHeight / 2;
        u32 StartScreenZ = 0;//INT16_MAX / 2;
      
        u32 PreviousScreenX = StartScreenX;
        u32 PreviousScreenY = StartScreenY;
        u32 PreviousScreenZ = StartScreenZ;
      
        u32 NextScreenX = PreviousScreenX;
        u32 NextScreenY = PreviousScreenY;
        u32 NextScreenZ = PreviousScreenZ;
        
        b32 PreviousDoorLeft = FALSE;
        b32 PreviousDoorRight = FALSE;
        b32 PreviousDoorTop = FALSE;
        b32 PreviousDoorBottom = FALSE;
        b32 PreviousDoorUpStairs = FALSE;
        b32 PreviousDoorDownStairs = FALSE;

        for (u32 ScreenIndex = 0; ScreenIndex < 2000; ScreenIndex++)
        {
            u32 CurrentScreenX = NextScreenX;
            u32 CurrentScreenY = NextScreenY;
            u32 CurrentScreenZ = NextScreenZ;
            b32 CurrentDoorLeft = FALSE;
            b32 CurrentDoorRight = FALSE;
            b32 CurrentDoorTop = FALSE;
            b32 CurrentDoorBottom = FALSE;
            b32 CurrentDoorUpStairs = FALSE;
            b32 CurrentDoorDownStairs = FALSE;

            Assert(RandomNumbersIndex < ArrayLength(RandomNumbersTable));
            u32 RandomChoice = 0;

            if
            (
                (PreviousDoorDownStairs && (CurrentScreenZ < PreviousScreenZ)) ||
                (PreviousDoorUpStairs && (CurrentScreenZ > PreviousScreenZ))
            )
            {
                // we can't have upstairs/downstairs movement,
                // previous room is directly above or below us
                RandomChoice = RandomNumbersTable[RandomNumbersIndex++] % 2;
            }
            else
            {
                RandomChoice = RandomNumbersTable[RandomNumbersIndex++] % 3;
            }

            if (RandomNumbersIndex == ArrayLength(RandomNumbersTable))
            {
                RandomNumbersIndex = 0;
            }

            if (RandomChoice == 0)
            {
                if (PreviousDoorLeft && (CurrentScreenX < PreviousScreenX))
                {
                    CurrentDoorTop = TRUE;
                    NextScreenY = CurrentScreenY + 1;
                }
                else
                {
                    CurrentDoorRight = TRUE;
                    NextScreenX = CurrentScreenX + 1;
                }
            }
            else if (RandomChoice == 1)
            {
                if (PreviousDoorBottom && (CurrentScreenY < PreviousScreenY))
                {
                    CurrentDoorRight = TRUE;
                    NextScreenX = CurrentScreenX + 1;
                }
                else
                {
                    CurrentDoorTop = TRUE;
                    NextScreenY = CurrentScreenY + 1;
                }
            }
            else if (RandomChoice == 2)
            {
                if (CurrentScreenZ == StartScreenZ)
                {
                    CurrentDoorUpStairs = TRUE;
                    NextScreenZ = StartScreenZ + 1;
                }
                else
                {
                    CurrentDoorDownStairs = TRUE;
                    NextScreenZ = StartScreenZ;
                }
            }

            if (CurrentScreenX < PreviousScreenX)
            {
                CurrentDoorRight = TRUE;
            }
            else if (CurrentScreenX > PreviousScreenX)
            {
                CurrentDoorLeft = TRUE;
            }
            else if (CurrentScreenY < PreviousScreenY)
            {
                CurrentDoorTop = TRUE;
            }
            else if (CurrentScreenY > PreviousScreenY)
            {
                CurrentDoorBottom = TRUE;
            }
            else if (CurrentScreenZ < PreviousScreenZ)
            {
                CurrentDoorUpStairs = TRUE;
            }
            else if (CurrentScreenZ > PreviousScreenZ)
            {
                CurrentDoorDownStairs = TRUE;
            }

            AddStandardRoom
            (
                GameState,
                (u32)(CurrentScreenX * TilesPerScreenWidth + TilesPerScreenWidth / 2.0f),
                (u32)(CurrentScreenY * TilesPerScreenHeight + TilesPerScreenHeight / 2.0f),
                CurrentScreenZ
            );

            for (u32 TileY = 0; TileY < TilesPerScreenHeight; TileY++)
            {
                for (u32 TileX = 0; TileX < TilesPerScreenWidth; TileX++)
                {
                    u32 AbsTileX = CurrentScreenX * TilesPerScreenWidth + TileX;
                    u32 AbsTileY = CurrentScreenY * TilesPerScreenHeight + TileY;
                    u32 AbsTileZ = CurrentScreenZ;
                    u32 TileValue = 1; // walkable

                    if (TileX == 0)
                    {
                        TileValue = 2; // wall
                        if (CurrentDoorLeft && (TileY == TilesPerScreenHeight / 2))
                        {
                            TileValue = 1; // door
                        }
                    }
                    if (TileX == (TilesPerScreenWidth - 1))
                    {
                        TileValue = 2; // wall
                        if (CurrentDoorRight && (TileY == TilesPerScreenHeight / 2))
                        {
                            TileValue = 1; // door
                        }
                    }
                    if (TileY == 0)
                    {
                        TileValue = 2; // wall
                        if (CurrentDoorBottom && (TileX == TilesPerScreenWidth / 2))
                        {
                            TileValue = 1; // door
                        }
                    }
                    if (TileY == (TilesPerScreenHeight - 1))
                    {
                        TileValue = 2; // wall
                        if (CurrentDoorTop && (TileX == TilesPerScreenWidth / 2))
                        {
                            TileValue = 1; // door
                        }
                    }

                    if (TileX == 2 && TileY == 5)
                    {
                        if (CurrentDoorUpStairs)
                        {
                            TileValue = 3; // upstairs door
                        }
                    }
                    else if (TileX == 2 && TileY == 2)
                    {
                        if (CurrentDoorDownStairs)
                        {
                            TileValue = 4; // downstairs door
                        }
                    }

                    if (TileValue == 2) // wall
                    {
                        AddWall(GameState, AbsTileX, AbsTileY, AbsTileZ);
                    }
                    else if (TileValue == 3)
                    {
                        AddStairs(GameState, AbsTileX, AbsTileY, AbsTileZ);
                    }
                    //else if (TileValue == 4)
                    //{
                    //    AddStairs(GameState, AbsTileX, AbsTileY, AbsTileZ);
                    //}
                }
            }

            PreviousScreenX = CurrentScreenX;
            PreviousScreenY = CurrentScreenY;
            PreviousScreenZ = CurrentScreenZ;
            PreviousDoorLeft = CurrentDoorLeft;
            PreviousDoorRight = CurrentDoorRight;
            PreviousDoorTop = CurrentDoorTop;
            PreviousDoorBottom = CurrentDoorBottom;
            PreviousDoorUpStairs = CurrentDoorUpStairs;
            PreviousDoorDownStairs = CurrentDoorDownStairs;
        }

#if 0
        // NOTE: Dummy filler low freq. entities
        while (GameState->StorageEntitiesCount < (ArrayLength(GameState->LowEntities) - 16))
        {
            u32 Coordinate = 1024 + GameState->StorageEntitiesCount;
            AddWall(GameState, Coordinate, Coordinate, Coordinate);
        }
#endif

        u32 CameraTileX = StartScreenX * TilesPerScreenWidth + TilesPerScreenWidth / 2;
        u32 CameraTileY = StartScreenY * TilesPerScreenHeight + TilesPerScreenHeight / 2;
        u32 CameraTileZ = StartScreenZ;

        GameState->CameraPosition = GetWorldPositionFromTilePosition(World, CameraTileX, CameraTileY, CameraTileZ, V3(0, 0, 0));

        AddMonster(GameState, CameraTileX + 2, CameraTileY + 2, CameraTileZ);

        for (u32 FamiliarIndex = 0; FamiliarIndex < 1; FamiliarIndex++)
        {
            i32 FamiliarXOffset = (RandomNumbersTable[RandomNumbersIndex++] % 6) - 3;
            if (RandomNumbersIndex == ArrayLength(RandomNumbersTable))
            {
                RandomNumbersIndex = 0;
            }
            
            i32 FamiliarYOffset = (RandomNumbersTable[RandomNumbersIndex++] % 4) - 2;
            if (RandomNumbersIndex == ArrayLength(RandomNumbersTable))
            {
                RandomNumbersIndex = 0;
            }

            if ((FamiliarXOffset != 0) || (FamiliarYOffset != 0))
            {
                AddFamiliar(GameState, CameraTileX - FamiliarXOffset, CameraTileY - FamiliarYOffset, CameraTileZ);
            }
        }
    }

    world *World = GameState->World;
    
    for (u32 ControllerIndex = 0; ControllerIndex < ArrayLength(GameInput->ControllerStates); ControllerIndex++)
    {
        game_controller_state *ControllerInput = GetController(GameInput, ControllerIndex);
        controlled_hero_input *ControlledHeroInput = GameState->ControllerToHeroInputMap + ControllerIndex;

        if (ControlledHeroInput->HeroEntityStorageIndex == 0)
        {
            if (ControllerInput->Start.IsDown)
            {
                *ControlledHeroInput = {};
                ControlledHeroInput->HeroEntityStorageIndex = AddPlayer(GameState).StorageIndex;
            }
        }
        else
        {
            ControlledHeroInput->InputJumpVelocity = 0;
            ControlledHeroInput->InputAcceleration = V3(0.0f, 0.0f, 0.0f);
            ControlledHeroInput->InputSwordDirection = V2(0.0f, 0.0f);

            if (ControllerInput->MovementIsAnalog)
            {
                ControlledHeroInput->InputAcceleration = 
                    V3(ControllerInput->StickAverageX, ControllerInput->StickAverageY, 0);
            }
            else
            {
                if (ControllerInput->MoveUp.IsDown)
                {
                    ControlledHeroInput->InputAcceleration.Y = 1.0f;
                }
                if (ControllerInput->MoveDown.IsDown)
                {
                    ControlledHeroInput->InputAcceleration.Y = -1.0f;
                }
                if (ControllerInput->MoveRight.IsDown)
                {
                    ControlledHeroInput->InputAcceleration.X = 1.0f;
                }
                if (ControllerInput->MoveLeft.IsDown)
                {
                    ControlledHeroInput->InputAcceleration.X = -1.0f;
                }
            }

            if (ControllerInput->Start.IsDown)
            {
                ControlledHeroInput->InputJumpVelocity = 2.0f;
            }

            if (ControllerInput->ActionUp.IsDown)
            {
                ControlledHeroInput->InputSwordDirection = V2(0, 1);
            }
            else if (ControllerInput->ActionDown.IsDown)
            {
                ControlledHeroInput->InputSwordDirection = V2(0, -1);
            }
            else if (ControllerInput->ActionRight.IsDown)
            {
                ControlledHeroInput->InputSwordDirection = V2(1, 0);
            }
            else if (ControllerInput->ActionLeft.IsDown)
            {
                ControlledHeroInput->InputSwordDirection = V2(-1, 0);
            }
        }
    }

    u32 CameraRegionTileSpanX = 17 * 3;
    u32 CameraRegionTileSpanY = 9 * 3;
    u32 CameraRegionTileSpanZ = 1;
    rectangle3 CameraRegionUpdateBounds = RectCenterDiameter
    (
        V3(0, 0, 0),
        GameState->World->TileSideInMeters * V3((f32)CameraRegionTileSpanX, (f32)CameraRegionTileSpanY, (f32)CameraRegionTileSpanZ)
    );

    memory_arena SimulationArena;
    InitializeMemoryArena(&SimulationArena, GameMemory->TransientStorageSize, GameMemory->TransientStorage);

    simulation_region *CameraSimulationRegion =
        BeginSimulation(GameState, World, &SimulationArena, GameState->CameraPosition, CameraRegionUpdateBounds, GameInput->TimeDeltaForFrame);

#if 1
    DrawRectangle(PixelBuffer, V2(0.0f, 0.0f), V2((f32)PixelBuffer->WidthInPixels, (f32)PixelBuffer->HeightInPixels), 0.5f, 0.5f, 0.5f, 1.0f);
#else
    DrawBitmap(&GameState->BackDropBitMap, PixelBuffer, 0, 0, 0, 0, 1);
#endif

    render_peice_group EntityRenderPeiceGroup = {};

    entity *CurrentEntity = CameraSimulationRegion->Entities;
    for
    (
        u32 EntityIndex = 0; 
        EntityIndex < CameraSimulationRegion->CurrentEntityCount;
        EntityIndex++, CurrentEntity++
    )
    {
        if (CurrentEntity->CanUpdate)
        {
            EntityRenderPeiceGroup.Count = 0;
            hero_bitmap_group *HeroBitmapGroup = &GameState->HeroBitmapGroups[CurrentEntity->BitmapFacingDirection];

            // TODO: ShadowBitMap alpha/position calculation is impercise, should be after postition update for all entities
            f32 ShadowAlphaFactor = 1.0f - 0.5f * CurrentEntity->Position.Z;
            if (ShadowAlphaFactor < 0)
            {
                ShadowAlphaFactor = 0;
            }

            entity_movement_parameters EntityMoveSpec = DefaultEntityMovementParameters();
            v3 EntityAcceleration = V3(0, 0, 0);

            switch (CurrentEntity->Type)
            {
            case ET_HERO:
            {
                for (u32 ControlledHeroInputIndex = 0; ControlledHeroInputIndex < ArrayLength(GameState->ControllerToHeroInputMap); ControlledHeroInputIndex++)
                {
                    controlled_hero_input *ControlledHeroInput = GameState->ControllerToHeroInputMap + ControlledHeroInputIndex;
                    if (ControlledHeroInput->HeroEntityStorageIndex == CurrentEntity->StorageIndex)
                    {
                        if (ControlledHeroInput->InputJumpVelocity != 0)
                        {
                            CurrentEntity->Velocity.Z = ControlledHeroInput->InputJumpVelocity;
                        }

                        EntityMoveSpec.NormalizeAcceleration = TRUE;
                        EntityMoveSpec.SpeedInXYPlane = 50.0f;
                        EntityMoveSpec.DragInXYPlane = 4.0f;

                        EntityAcceleration = ControlledHeroInput->InputAcceleration;

                        if ((ControlledHeroInput->InputSwordDirection.X != 0) || (ControlledHeroInput->InputSwordDirection.Y != 0))
                        {
                            entity *SwordEntity = CurrentEntity->SwordEntityReference.Entity;
                            if (SwordEntity && IsEntityFlagSet(SwordEntity, EF_NON_SPATIAL))
                            {
                                MakeEntitySpatial(SwordEntity,
                                                  CurrentEntity->Position,
                                                  CurrentEntity->Velocity + 5.0f * V3(ControlledHeroInput->InputSwordDirection, 0));
                                
                                SwordEntity->MovementDistanceLimit = 5.0f;
                                
                                AddEntityCollisionRule(GameState, SwordEntity->StorageIndex, CurrentEntity->StorageIndex, FALSE);
                            }
                        }
                    }
                }

                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &GameState->ShadowBitMap, V3(0, 0, 0), HeroBitmapGroup->Alignment, ShadowAlphaFactor, 0.0f);
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &HeroBitmapGroup->Torso, V3(0, 0, 0), HeroBitmapGroup->Alignment, 1, 1.0f);
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &HeroBitmapGroup->Cape, V3(0, 0, 0), HeroBitmapGroup->Alignment, 1, 1.0f);
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &HeroBitmapGroup->Head, V3(0, 0, 0), HeroBitmapGroup->Alignment, 1, 1.0f);
                DrawHitpoints(GameState, &EntityRenderPeiceGroup, CurrentEntity);
            } break;

            case ET_WALL:
            {
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &GameState->TreeBitMap, V3(0, 0, 0), V2(40, 80), 1, 1.0f);
            } break;

            case ET_STAIRS:
            {
                PushRectangleRenderPiece(GameState, &EntityRenderPeiceGroup, V3(0, 0, 0), CurrentEntity->WalkableDiameter, V4(1, 0.5f, 0, 1), 0);
                PushRectangleRenderPiece(GameState, &EntityRenderPeiceGroup, V3(0, 0, CurrentEntity->WalkableHeight), CurrentEntity->WalkableDiameter, V4(1, 1, 0, 1), 0);
            } break;

            case ET_SWORD:
            {
                EntityMoveSpec.NormalizeAcceleration = FALSE;
                EntityMoveSpec.SpeedInXYPlane = 0;
                EntityMoveSpec.DragInXYPlane = 0;
                
                EntityAcceleration = V3(0, 0, 0);

                if (CurrentEntity->MovementDistanceLimit == 0)
                {
                    ClearAllEnrityCollisionRules(GameState, CurrentEntity->StorageIndex);
                    MakeEntityNonSpatial(CurrentEntity);
                }

                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &GameState->ShadowBitMap, V3(0, 0, 0), HeroBitmapGroup->Alignment, ShadowAlphaFactor, 0.0f);
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &GameState->SwordBitMap, V3(0, 0, 0), V2(29, 10), 1, 1.0f);
            } break;

            case ET_MONSTER:
            {
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &HeroBitmapGroup->Torso, V3(0, 0, 0), HeroBitmapGroup->Alignment, 1, 1.0f);
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &GameState->ShadowBitMap, V3(0, 0, 0), HeroBitmapGroup->Alignment, ShadowAlphaFactor, 0.0f);
                DrawHitpoints(GameState, &EntityRenderPeiceGroup, CurrentEntity);
            } break;

            case ET_FAMILIAR:
            {
                EntityAcceleration = V3(0, 0, 0);

                EntityMoveSpec.NormalizeAcceleration = TRUE;
                EntityMoveSpec.SpeedInXYPlane = 50.0f;
                EntityMoveSpec.DragInXYPlane = 4.0f;

#if 0
                entity *ClosestHeroEntity = 0;
                f32 ClosestHeroDistanceSquared = 100.0f;

                entity *TestEntity = CameraSimulationRegion->Entities;
                for (u32 TestEntityIndex = 0; TestEntityIndex < CameraSimulationRegion->CurrentEntityCount; TestEntityIndex++, TestEntity++)
                {
                    if (TestEntity->Type == ET_HERO)
                    {
                        f32 ThisHeroDistanceSquared = LengthSquared(TestEntity->GroundPoint - CurrentEntity->GroundPoint);
                        if (ThisHeroDistanceSquared < ClosestHeroDistanceSquared)
                        {
                            ClosestHeroDistanceSquared = ThisHeroDistanceSquared;
                            ClosestHeroEntity = TestEntity;
                        }
                    }
                }

                if (ClosestHeroEntity && (ClosestHeroDistanceSquared > Square(3.0f)))
                {
                    f32 AccelerationMultiplier = 0.5f;
                    f32 OneOverClosestHeroDistance = 1.0f / SquareRoot(ClosestHeroDistanceSquared);

                    EntityAcceleration =
                        AccelerationMultiplier *
                        OneOverClosestHeroDistance *
                        (ClosestHeroEntity->GroundPoint - CurrentEntity->GroundPoint);
                }
#endif
                CurrentEntity->BobbingSinParameter += 5.0f * GameInput->TimeDeltaForFrame;
                if (CurrentEntity->BobbingSinParameter > 2.0f * PI32)
                {
                    CurrentEntity->BobbingSinParameter -= 2.0f * PI32;
                }
                f32 BobbingSin = Sin(CurrentEntity->BobbingSinParameter);
                ShadowAlphaFactor = 0.5f * ShadowAlphaFactor + 0.2f * BobbingSin;

                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &GameState->ShadowBitMap, V3(0, 0, 0), HeroBitmapGroup->Alignment, ShadowAlphaFactor, 0.0f);
                PushBitmapRenderPiece(GameState, &EntityRenderPeiceGroup, &HeroBitmapGroup->Head, V3(0, 0, 0.25f * BobbingSin), HeroBitmapGroup->Alignment, 1, 1.0f);
            } break;

            case ET_SPACE:
            {
                for (u32 MeshIndex = 0; MeshIndex < CurrentEntity->CollisionMeshGroup->MeshCount; MeshIndex++)
                {
                    entity_collision_mesh *CurrnetCollisionMesh =  CurrentEntity->CollisionMeshGroup->Meshes + MeshIndex;

                    PushRectangleOutlineRenderPieces(GameState, &EntityRenderPeiceGroup, V3(CurrnetCollisionMesh->Offset.XY, 0),
                                                     CurrnetCollisionMesh->Diameter.XY, V4(0, 0.5f, 1.0f, 1.0f), 0);
                }
            } break;

            default:
            {
                InvalidCodepath;
            } break;
            }

            if
            (
                !IsEntityFlagSet(CurrentEntity, EF_NON_SPATIAL) &&
                IsEntityFlagSet(CurrentEntity, EF_MOVEABLE)
            )
            {
                MoveEntity(GameState, CameraSimulationRegion, CurrentEntity, EntityAcceleration, GameInput->TimeDeltaForFrame, &EntityMoveSpec);
            }

            render_piece *RenderPiece = EntityRenderPeiceGroup.Peices;
            for (u32 PieceIndex = 0; PieceIndex < EntityRenderPeiceGroup.Count; PieceIndex++, RenderPiece++)
            {
                v3 EntityBasePoint = GetEntityGroundPoint(CurrentEntity);

                f32 ZFudge = 1 + 0.1f * (EntityBasePoint.Z + RenderPiece->Offset.Z);

                v2 ScreenCenterOffset = 0.5f * V2((f32)PixelBuffer->WidthInPixels, (f32)PixelBuffer->HeightInPixels);
            
                v2 EntityGroundPoint = 
                {
                    ScreenCenterOffset.X + EntityBasePoint.X * ZFudge * GameState->PixelsToMetersRatio,
                    ScreenCenterOffset.Y - EntityBasePoint.Y * ZFudge * GameState->PixelsToMetersRatio
                };
                f32 EntityZ = -EntityBasePoint.Z * GameState->PixelsToMetersRatio;

                v2 Center = V2
                (
                    EntityGroundPoint.X + RenderPiece->Offset.X,
                    EntityGroundPoint.Y + RenderPiece->Offset.Y + RenderPiece->EntityJumpZCoefficient * EntityZ
                );

                if (RenderPiece->Bitmap)
                {
                    DrawBitmap(RenderPiece->Bitmap, PixelBuffer, Center.X, Center.Y, RenderPiece->Color.Alpha);
                }
                else
                {
                    v2 DimensionInPixels = RenderPiece->Dimensions * GameState->PixelsToMetersRatio;
                    DrawRectangle
                    (
                        PixelBuffer,
                        Center - 0.5f * DimensionInPixels, Center + 0.5f * DimensionInPixels,
                        RenderPiece->Color.Red, RenderPiece->Color.Green, RenderPiece->Color.Blue, RenderPiece->Color.Alpha
                    );
                }
            }
        }
    }

    world_position OriginWorldPosition = {};
    v3 Diff = SubtractPositions(CameraSimulationRegion->World, &OriginWorldPosition, &CameraSimulationRegion->Origin);
    v2 Origin = V2(Diff.X, Diff.Y);
    DrawRectangle(PixelBuffer, Origin, Origin + V2(10.0f, 10.0f), 1.0f, 1.0f, 0.0f, 1.0f);

    EndSimulation(CameraSimulationRegion, GameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
//void GameGetSoundSamples(thread_context *ThreadContext, game_sound_request *SoundRequest, game_memory *GameMemory)
{
    game_state *GameState = (game_state *)GameMemory->PermanentStorage;
    GameOutputSound(GameState, SoundRequest, 400);
}
