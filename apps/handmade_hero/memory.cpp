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