#pragma once

#define PushStruct(Arena, DataType) (DataType far *)PushOntoMemoryArena((Arena), sizeof(DataType))
#define PushArray(Arena, Count, DataType) (DataType far *)PushOntoMemoryArena((Arena), (Count) * sizeof(DataType))

typedef struct
{
    u32 Size;
    u32 Used;
    u8 far *BaseAddress;
} memory_arena;