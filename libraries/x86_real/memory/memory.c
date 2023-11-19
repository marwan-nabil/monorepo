// ==========================
// Memory set implementations
// ==========================
void MemorySetNear(void *Destination, u8 Value, u32 Size)
{
    for (u16 Index = 0; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = Value;
    }
}

void MemorySetFar(void *Destination, u8 Value, u32 Size)
{
    for (u16 Index = 0; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = Value;
    }
}

// ===========================
// Memory zero implementations
// ===========================
void MemoryZeroNear(void *Destination, u32 Size)
{
    MemorySetNear(Destination, 0, Size);
}

void MemoryZeroFar(void *Destination, u32 Size)
{
    MemorySetFar(Destination, 0, Size);
}

// ===========================
// Memory Copy implementations
// ===========================
void MemoryCopyNearToNear(void *Destination, void *Source, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = ((u8 *)Source)[Index];
    }
}

void MemoryCopyFarToFar(void *Destination, void *Source, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = ((u8 *)Source)[Index];
    }
}

void MemoryCopyFarToNear(void *Destination, void *Source, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = ((u8 *)Source)[Index];
    }
}

void MemoryCopyNearToFar(void *Destination, void *Source, u32 Size)
{
    u32 Index = 0;
    for (; Index < Size; Index++)
    {
        ((u8 *)Destination)[Index] = ((u8 *)Source)[Index];
    }
}

// ==============================
// Memory compare implementations
// ==============================
i16 MemoryCompareNearToNear(void *Source1, void *Source2, u32 Size)
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

i16 MemoryCompareNearToFar(void *Source1, void *Source2, u32 Size)
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

i16 MemoryCompareFarToFar(void *Source1, void *Source2, u32 Size)
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

// ======================
// other memory functions
// ======================
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