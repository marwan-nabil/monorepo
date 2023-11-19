#pragma once

#define PushStruct(Arena, DataType) \
    (DataType *)PushOntoMemoryArena((Arena), sizeof(DataType))
#define PushArray(Arena, Count, DataType) \
    (DataType *)PushOntoMemoryArena((Arena), (Count) * sizeof(DataType))

typedef struct
{
    u32 Size;
    u32 Used;
    u8 *BaseAddress;
} memory_arena;