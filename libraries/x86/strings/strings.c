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

long int StringLength(char *String)
{
    long int Count = 0;
    while (*String++)
    {
        ++Count;
    }
    return Count;
}

void PrintFormattedNumberUnsigned(u64 Number, u32 Radix)
{
    const char HexCharacters[] = "0123456789abcdef";

    char LocalStringBuffer[32];
    MemoryZero((void *)LocalStringBuffer, 32);

    i32 StringBufferPosition = 0;

    do
    {
        u64 Remainder = Number % Radix;
        Number /= Radix;

        LocalStringBuffer[StringBufferPosition++] = HexCharacters[Remainder];
    } while (Number > 0);

    while (--StringBufferPosition >= 0)
    {
        BIOSPrintCharacter(LocalStringBuffer[StringBufferPosition]);
    }
}

void PrintFormattedNumberSigned(u64 Number, u32 Radix)
{
    if (Number < 0)
    {
        BIOSPrintCharacter('-');
        PrintFormattedNumberUnsigned(-Number, Radix);
    }
    else
    {
        PrintFormattedNumberUnsigned(Number, Radix);
    }
}

void PrintFormatted(const char *FormatString, ...)
{
    va_list ArgumentsList;
    va_start(ArgumentsList, FormatString);

    printf_state FormatStringState = PRINTF_STATE_NORMAL;
    printf_length_type LengthType = PRINTF_LENGTH_TYPE_DEFAULT;
    u32 Radix = 10;
    b8 IsSigned = FALSE;
    b8 IsNumber = FALSE;

    while (*FormatString)
    {
        switch (FormatStringState)
        {
            case PRINTF_STATE_NORMAL:
            {
                if (*FormatString == '%')
                {
                    FormatStringState = PRINTF_STATE_CHECK_LENGTH;
                }
                else
                {
                    BIOSPrintCharacter(*FormatString);
                }
                FormatString++;
            } break;

            case PRINTF_STATE_CHECK_LENGTH:
            {
                if (*FormatString == 'h')
                {
                    LengthType = PRINTF_LENGTH_TYPE_SHORT;
                    FormatStringState = PRINTF_STATE_SHORT_LENGTH;
                    FormatString++;
                }
                else if (*FormatString == 'l')
                {
                    LengthType = PRINTF_LENGTH_TYPE_LONG;
                    FormatStringState = PRINTF_STATE_LONG_LENGTH;
                    FormatString++;
                }
                else
                {
                    FormatStringState = PRINTF_STATE_CHECK_SPECIFIER;
                }
            } break;

            case PRINTF_STATE_SHORT_LENGTH:
            {
                if (*FormatString == 'h')
                {
                    LengthType = PRINTF_LENGTH_TYPE_SHORT_SHORT;
                    FormatStringState = PRINTF_STATE_CHECK_SPECIFIER;
                    FormatString++;
                }
                else
                {
                    FormatStringState = PRINTF_STATE_CHECK_SPECIFIER;
                }
            } break;

            case PRINTF_STATE_LONG_LENGTH:
            {
                if (*FormatString == 'l')
                {
                    LengthType = PRINTF_LENGTH_TYPE_LONG_LONG;
                    FormatStringState = PRINTF_STATE_CHECK_SPECIFIER;
                    FormatString++;
                }
                else
                {
                    FormatStringState = PRINTF_STATE_CHECK_SPECIFIER;
                }
            } break;

            case PRINTF_STATE_CHECK_SPECIFIER:
            {
                switch (*FormatString)
                {
                    case 'c':
                    {
                        BIOSPrintCharacter((char)va_arg(ArgumentsList, i32));
                    } break;

                    case 's':
                    {
                        BIOSPrintString(va_arg(ArgumentsList, char *));
                    } break;

                    case '%':
                    {
                        BIOSPrintCharacter('%');
                    } break;

                    case 'd':
                    case 'i':
                    {
                        Radix = 10;
                        IsSigned = TRUE;
                        IsNumber = TRUE;
                    } break;

                    case 'u':
                    {
                        Radix = 10;
                        IsSigned = FALSE;
                        IsNumber = TRUE;
                    } break;

                    case 'X':
                    case 'x':
                    case 'p':
                    {
                        Radix = 16;
                        IsSigned = FALSE;
                        IsNumber = TRUE;
                    } break;

                    case 'o':
                    {
                        Radix = 8;
                        IsSigned = FALSE;
                        IsNumber = TRUE;
                    } break;

                    default:
                    {
                    } break;
                }

                if (IsNumber)
                {
                    if (IsSigned)
                    {
                        switch (LengthType)
                        {
                            case PRINTF_LENGTH_TYPE_SHORT_SHORT:
                            case PRINTF_LENGTH_TYPE_SHORT:
                            case PRINTF_LENGTH_TYPE_DEFAULT:
                            {
                                PrintFormattedNumberSigned(va_arg(ArgumentsList, i32), Radix);
                            } break;

                            case PRINTF_LENGTH_TYPE_LONG:
                            case PRINTF_LENGTH_TYPE_LONG_LONG:
                            {
                                PrintFormattedNumberSigned(va_arg(ArgumentsList, i64), Radix);
                            } break;
                        }
                    }
                    else
                    {
                        switch (LengthType)
                        {
                            case PRINTF_LENGTH_TYPE_SHORT_SHORT:
                            case PRINTF_LENGTH_TYPE_SHORT:
                            case PRINTF_LENGTH_TYPE_DEFAULT:
                            {
                                PrintFormattedNumberUnsigned(va_arg(ArgumentsList, u32), Radix);
                            } break;

                            case PRINTF_LENGTH_TYPE_LONG:
                            case PRINTF_LENGTH_TYPE_LONG_LONG:
                            {
                                PrintFormattedNumberUnsigned(va_arg(ArgumentsList, u64), Radix);
                            } break;
                        }
                    }
                }

                FormatStringState = PRINTF_STATE_NORMAL;
                LengthType = PRINTF_LENGTH_TYPE_DEFAULT;
                Radix = 10;
                IsSigned = FALSE;
                IsNumber = FALSE;

                FormatString++;
            } break;
        }
    }
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