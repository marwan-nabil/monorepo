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

void MemorySet(void *Destination, u8 Value, u32 Size)
{
    for (u16 Index = 0; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = Value;
    }
}

void FarMemorySet(void far *Destination, u8 Value, u32 Size)
{
    for (u16 Index = 0; Index < Size; Index++)
    {
        ((u8 far *)Destination)[Index] = Value;
    }
}

i16 MemoryCompare(void *Source1, void *Source2, u32 Size)
{
    for (u16 Index = 0; Index < Size; Index++)
    {
        if
        (
            ((u8 *)Source1)[Index] !=
            ((u8 *)Source2)[Index]
        )
        {
            return 1;
        }
    }

    return 0;
}

i16 FarMemoryCompare(void far *Source1, void far *Source2, u32 Size)
{
    for (u16 Index = 0; Index < Size; Index++)
    {
        if
        (
            ((u8 far *)Source1)[Index] !=
            ((u8 far *)Source2)[Index]
        )
        {
            return 1;
        }
    }

    return 0;
}

u32 AlignPointer(u32 Pointer, u32 Alignment)
{
    if (Alignment == 0)
    {
        return Pointer;
    }

    u32 Remainder = Pointer % Alignment;
    if (Remainder > 0)
    {
        return Pointer + (Alignment - Remainder);
    }
    else
    {
        return Pointer;
    }
}