void MemoryZero(void *Destination, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = 0;
    }
}

void FarMemoryZero(void far *Destination, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = 0;
    }
}

void MemoryCopy(void *Destination, void *Source, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = ((u8 *)Source)[Index];
    }
}

void FarMemoryCopy(void far *Destination, void far *Source, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = ((u8 *)Source)[Index];
    }
}