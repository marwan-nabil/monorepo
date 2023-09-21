inline void
CombineStrings
(
    size_t SourceASize, size_t SourceBSize, size_t DestinationSize,
    char *SourceA, char *SourceB, char *Destination
)
{
    Assert((SourceASize + SourceBSize) < DestinationSize);

    for (u32 Index = 0; Index < SourceASize; Index++)
    {
        *Destination++ = *SourceA++;
    }

    for (u32 Index = 0; Index < SourceBSize; Index++)
    {
        *Destination++ = *SourceB++;
    }

    *Destination++ = '\0';
}

inline u32
StringLength(char *String)
{
    u32 Count = 0;
    while (*String++)
    {
        ++Count;
    }
    return Count;
}

inline void
InjectEscapeSlashes(char *SourceString, char *DestinationString)
{
    while (*SourceString)
    {
        if (*SourceString == '\\')
        {
            *DestinationString++ = '\\';
        }
        *DestinationString++ = *SourceString++;
    }
}