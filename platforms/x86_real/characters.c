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

void PrintCharacter(char Character)
{
    X86_PrintCharacter(Character, 0);
}