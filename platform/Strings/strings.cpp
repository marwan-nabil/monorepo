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
InjectEscapeSlashes(char *SourceString, char *DestinationString, u32 DestinationSize)
{
    u32 WrittenCharacters = 0;
    while (*SourceString)
    {
        if (*SourceString == '\\')
        {
            DestinationString[WrittenCharacters++] = '\\';
            if (WrittenCharacters >= DestinationSize)
            {
                return;
            }
        }

        DestinationString[WrittenCharacters++] = *SourceString++;
        if (WrittenCharacters > DestinationSize)
        {
            return;
        }
    }
}

inline u32
GetFirstCharacterIndex(char *String, u32 StringLength, char Character)
{
    for (u32 CharacterIndex = 0; CharacterIndex < StringLength; CharacterIndex++)
    {
        if (String[CharacterIndex] == Character)
        {
            return CharacterIndex;
        }
    }

    return UINT32_MAX;
}

inline u32
GetLastCharacterIndex(char *String, u32 StringLength, char Character)
{
    Assert(StringLength);

    for (i32 CharacterIndex = StringLength - 1; CharacterIndex >= 0; CharacterIndex--)
    {
        if (String[CharacterIndex] == Character)
        {
            return CharacterIndex;
        }
    }

    return UINT32_MAX;
}

// inline void
// GetFileNameAndExtensionFromString
// (
//     char *SourceString, u32 SourceStringSize,
//     char *FileName, u32 FileNameSize,
//     char *FileExtension, u32 FileExtensionSize
// )
// {
//     u32 SourceStringLength = StringLength(SourceString);
//     u32 DotIndex = GetLastCharacterIndex(SourceString, SourceStringLength);

//     for (u32 CharacterIndex = 0; CharacterIndex < DotIndex; )
//     {
//     }

//     memcpy(FileName, SourceString, DotIndex);
//     memcpy(FileExtension, SourceString[DotIndex + 1], );

//     while (*SourceString)
//     {

//     }
// }