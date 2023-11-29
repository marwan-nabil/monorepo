b8 IsCharacterLowerCase(char Character)
{
    if ((Character >= 'a') && (Character <= 'z'))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

char ConvertCharacterToUpperCase(char Character)
{
    if (IsCharacterLowerCase(Character))
    {
        return 'A' + (Character - 'a');
    }
    else
    {
        return Character;
    }
}

const char *GetCharacterPointer(const char *String, char Character)
{
    if (String == NULL)
    {
        return NULL;
    }

    while (*String)
    {
        if (*String == Character)
        {
            return String;
        }

        ++String;
    }

    return NULL;
}

i16 StringCompare(char *String1, char *String2, u32 ComparisonRange)
{
    if
    (
        (String1 == NULL) ||
        (String2 == NULL)
    )
    {
        return -1;
    }

    for (u32 Index = 0; Index < ComparisonRange; Index++)
    {
        if (String1[Index] != String2[Index])
        {
            return -1;
        }
    }

    return 0;
}

void StringConcatenate(char *Destination, u32 Size, char *Source)
{
    if ((Destination == NULL) || (Source == NULL))
    {
        return;
    }

    u32 CharIndex = 0;

    while (Destination[CharIndex])
    {
        CharIndex++;
    }

    for (; CharIndex < (Size - 1); CharIndex++)
    {
        if (*Source)
        {
            Destination[CharIndex] = *Source++;
        }
        else
        {
            break;
        }
    }
    Destination[CharIndex] = '\0';
}

u32 StringLength(char *String)
{
    u32 Count = 0;
    while (*String++)
    {
        ++Count;
    }
    return Count;
}

void FillFixedSizeStringBuffer(char *Buffer, u32 BufferSize, char *SourceString)
{
    if (StringLength(SourceString) >= BufferSize)
    {
        MemoryCopy(Buffer, SourceString, BufferSize);
    }
    else
    {
        StringConcatenate(Buffer, BufferSize, SourceString);
    }
}