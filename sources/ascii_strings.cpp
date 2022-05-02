struct string_length_cache_entry
{
    const char *String;
    size_t StringLength;
    u8 Freshness;
};

string_length_cache_entry StringLengthsCache[4] = {};

inline size_t
StringLength(const char *String)
{
    size_t Length = 0u;
    bool32 FoundInCache = false;

    for (u32 CacheIndex = 0u; CacheIndex < ArrayLength(StringLengthsCache); CacheIndex++)
    {
        string_length_cache_entry *CurrentEntry = &StringLengthsCache[CacheIndex];
        if (CurrentEntry->String == String)
        {
            CurrentEntry->Freshness = 255u;
            Length = CurrentEntry->StringLength;
            FoundInCache = true;
            break;
        }
    }

    if (FoundInCache == false)
    {
        for (u32 CharacterIndex = 0u; CharacterIndex < MAX_PATH; CharacterIndex++)
        {
            if (String[CharacterIndex] == '\0')
            {
                break;
            }
            else
            {
                Length++;
            }
        }
    }

    for (u32 CacheIndex = 0u; CacheIndex < ArrayLength(StringLengthsCache); CacheIndex++)
    {
        string_length_cache_entry *CurrentEntry = &StringLengthsCache[CacheIndex];
        bool32 NewEntryInsertedAlready = FoundInCache;

        if (CurrentEntry->String == NULL)
        {
            if (NewEntryInsertedAlready == false)
            {
                CurrentEntry->String = String;
                CurrentEntry->StringLength = Length;
                CurrentEntry->Freshness = 255u;
                NewEntryInsertedAlready = true;
            }
        }
        else 
        {
            if ((CurrentEntry->String != String) && (CurrentEntry->Freshness > 0u))
            {
                CurrentEntry->Freshness--;
            }
        }
    }

    return Length;
}

inline void
CopyString(char *DestinationBuffer, size_t DestinationSize, const char *SourceString)
{
    StringCchCopyA(DestinationBuffer, DestinationSize, SourceString);
}

inline void
AppendString(char *DestinationBuffer, size_t DestinationSize, const char *SourceString)
{
    StringCchCatA(DestinationBuffer, DestinationSize, SourceString);
}

bool32 ValidateStringLength(char *Path, u32 MaxAllowedLength, const char *ErrorMessage)
{
    bool32 Result = false;
    if (StringLength(Path) > MaxAllowedLength)
    {
        printf("ERROR ValidateStringLength(): %s\n", ErrorMessage);
        Result = true;
    }
    return Result;
}
