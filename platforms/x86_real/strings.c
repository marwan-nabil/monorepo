char *StringCopy(char *Destination, const char *Source)
{
    char *Result = Destination;

    if (Destination == NULL)
    {
        return Result;
    }

    if (Source == NULL)
    {
        *Destination = '\0';
        return Result;
    }

    while (*Source)
    {
        *Destination++ = *Source++;
    }
    
    *Destination = '\0';
    return Result;
}

long int StringLengthFar(char far *String)
{
    long int Count = 0;
    while (*String++)
    {
        ++Count;
    }
    return Count;
}

long int StringLength(char far *String)
{
    long int Count = 0;
    while (*String++)
    {
        ++Count;
    }
    return Count;
}

void PrintString(const char *String)
{
    while (*String)
    {
        PrintCharacter(*String);
        String++;
    }
}

void PrintFarString(const char far *String)
{
    while (*String)
    {
        PrintCharacter(*String);
        String++;
    }
}

i16 *PrintFormattedNumber(i16 *ArgumentPointer, printf_length_type LengthType, b8 IsSigned, u32 Radix)
{
    const char HexCharacters[] = "0123456789abcdef";
    char LocalStringBuffer[32];
    u64 Number;
    i32 NumberSign = 1;
    i32 StringBufferPosition = 0;

    MemoryZero((void *)LocalStringBuffer, 32);

    switch (LengthType)
    {
        case PRINTF_LENGTH_TYPE_SHORT_SHORT:
        case PRINTF_LENGTH_TYPE_SHORT:
        case PRINTF_LENGTH_TYPE_DEFAULT:
        {
            if (IsSigned)
            {
                i32 Value = *ArgumentPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    NumberSign = -1;
                }
                Number = (u64)Value;
            }
            else
            {
                Number = *(u16 *)ArgumentPointer;
            }
            ArgumentPointer++;
        } break;

        case PRINTF_LENGTH_TYPE_LONG:
        {
            if (IsSigned)
            {
                i32 Value = *(i32 *)ArgumentPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    NumberSign = -1;
                }
                Number = (u64)Value;
            }
            else
            {
                Number = *(u32 *)ArgumentPointer;
            }
            ArgumentPointer += 2;
        } break;

        case PRINTF_LENGTH_TYPE_LONG_LONG:
        {
            if (IsSigned)
            {
                i64 Value = *(i64 *)ArgumentPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    NumberSign = -1;
                }
                Number = (u64)Value;
            }
            else
            {
                Number = *(u64 *)ArgumentPointer;
            }
            ArgumentPointer += 4;
        } break;
    }

    do
    {
        u32 Remainder;
        X86_DivideU64ByU32(Number, Radix, &Number, &Remainder);
        LocalStringBuffer[StringBufferPosition++] = HexCharacters[Remainder];
    } while (Number > 0);

    if (IsSigned && (NumberSign < 0))
    {
        LocalStringBuffer[StringBufferPosition++] = '-';
    }

    StringBufferPosition--;
    while (StringBufferPosition >= 0)
    {
        PrintCharacter(LocalStringBuffer[StringBufferPosition--]);
    }

    return ArgumentPointer;
}

void _cdecl PrintFormatted(const char *FormatString, ...)
{
    i16 *ArgumentPointer = (i16 *)&FormatString;
    printf_state FormatStringState = PRINTF_STATE_NORMAL;
    printf_length_type LengthType = PRINTF_LENGTH_TYPE_DEFAULT;
    u32 Radix = 10;
    b8 IsSigned = FALSE;

    ArgumentPointer++;

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
                    PrintCharacter(*FormatString);
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
                        PrintCharacter((char)*ArgumentPointer);
                        ArgumentPointer++;
                    } break;

                    case 's':
                    {
                        if
                        (
                            (LengthType == PRINTF_LENGTH_TYPE_LONG) ||
                            (LengthType == PRINTF_LENGTH_TYPE_LONG_LONG)
                        )
                        {
                            PrintFarString(*(const char far **)ArgumentPointer);
                            ArgumentPointer += 2;
                        }
                        else
                        {
                            PrintString(*(const char **)ArgumentPointer);
                            ArgumentPointer++;
                        }
                    } break;

                    case '%':
                    {
                        PrintCharacter('%');
                    } break;

                    case 'd':
                    case 'i':
                    {
                        Radix = 10;
                        IsSigned = TRUE;
                        ArgumentPointer = PrintFormattedNumber(ArgumentPointer, LengthType, IsSigned, Radix);
                    } break;

                    case 'u':
                    {
                        Radix = 10;
                        IsSigned = FALSE;
                        ArgumentPointer = PrintFormattedNumber(ArgumentPointer, LengthType, IsSigned, Radix);
                    } break;

                    case 'X':
                    case 'x':
                    case 'p':
                    {
                        Radix = 16;
                        IsSigned = FALSE;
                        ArgumentPointer = PrintFormattedNumber(ArgumentPointer, LengthType, IsSigned, Radix);
                    } break;

                    case 'o':
                    {
                        Radix = 8; IsSigned = FALSE;
                        ArgumentPointer = PrintFormattedNumber(ArgumentPointer, LengthType, IsSigned, Radix);
                    } break;

                    default:
                    {
                    } break;
                }

                FormatStringState = PRINTF_STATE_NORMAL;
                LengthType = PRINTF_LENGTH_TYPE_DEFAULT;
                Radix = 10;
                IsSigned = FALSE;

                FormatString++;
            } break;
        }
    }
}