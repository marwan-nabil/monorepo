#pragma once

#define PushStruct(Arena, DataType) (DataType *)PushOntoMemoryArena((Arena), sizeof(DataType))
#define PushArray(Arena, Count, DataType) (DataType *)PushOntoMemoryArena((Arena), (Count) * sizeof(DataType))
#define ZeroStruct(Struct) ZeroMemory(&(Struct), sizeof(Struct))

struct memory_arena
{
    size_t Size;
    size_t Used;
    u8 *Base;
};