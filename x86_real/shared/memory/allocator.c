void InitializeMemoryArena(memory_arena far *Arena, u32 Size, void far *BaseAddress)
{
    Arena->Size = Size;
    Arena->Used = 0;
    Arena->BaseAddress = (u8 far *)BaseAddress;
}

void far *PushOntoMemoryArena(memory_arena far *Arena, u32 PushSize)
{
    void far *Result = Arena->BaseAddress + Arena->Used;
    Arena->Used += PushSize;
    return Result;
}