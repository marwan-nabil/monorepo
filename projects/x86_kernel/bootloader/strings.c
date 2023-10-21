void PutCharacter(char Character)
{
    x86_PrintCharacter(Character, 0);
}

void PutString(const char *String)
{
    while (*String)
    {
        PutCharacter(*String);
        String++;
    }
}

void PutFarString(const char far *String)
{
    while(*String)
    {
        PutCharacter(*String);
        String++;
    }
}

i32 *PrintNumber(int *ArgPointer, printf_length_type LengthType, b8 Signed, int Radix)
{
    const char HexCharacters[] = "0123456789abcdef";

    char LocalStringBuffer[32];
    unsigned long long Number;
    int Sign = 1;
    int BufferPosition = 0;

    switch (LengthType)
    {
        case PRINTF_LENGTH_TYPE_SHORT_SHORT:
        case PRINTF_LENGTH_TYPE_SHORT:
        case PRINTF_LENGTH_TYPE_DEFAULT:
        {
            if (Signed)
            {
                int Value = *ArgPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    Sign = -1;
                }
                Number = (unsigned long long)Value;
            }
            else
            {
                Number = *(unsigned int *)ArgPointer;
            }
            ArgPointer++;
        } break;

        case PRINTF_LENGTH_TYPE_LONG:
        {
            if (Signed)
            {
                long int Value = *(long int *)ArgPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    Sign = -1;
                }
                Number = (unsigned long long)Value;
            }
            else
            {
                Number = *(unsigned long int *)ArgPointer;
            }
            ArgPointer += 2;
        } break;

        case PRINTF_LENGTH_TYPE_LONG_LONG:
        {
            if (Signed)
            {
                long long int Value = *(long long int *)ArgPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    Sign = -1;
                }
                Number = (unsigned long long)Value;
            }
            else
            {
                Number = *(unsigned long long *)ArgPointer;
            }
            ArgPointer += 4;
        } break;
    }

    do
    {
        u32 Remainder;
        x86_Divide64By32(Number, Radix, &Number, &Remainder);
        LocalStringBuffer[BufferPosition++] = HexCharacters[Remainder];
    } while (Number > 0);

    if (Signed && (Sign < 0))
    {
        LocalStringBuffer[BufferPosition++] = '-';
    }

    while (--BufferPosition >= 0)
    {
        PutCharacter(LocalStringBuffer[BufferPosition]);
    }

    return ArgPointer;
}

void _cdecl PrintF(const char *FormatString, ...)
{
    int *ArgPointer = (int *)&FormatString;
    printf_state State = PRINTF_STATE_NORMAL;
    printf_length_type LengthType = PRINTF_LENGTH_TYPE_DEFAULT;
    int Radix = 10;
    b8 Signed = FALSE;

    ArgPointer++;

    while (*FormatString)
    {
        switch (State)
        {
            case PRINTF_STATE_NORMAL:
            {
                if (*FormatString == '%')
                {
                    State = PRINTF_STATE_CHECK_LENGTH;
                }
                else
                {
                    PutCharacter(*FormatString);
                }
                FormatString++;
            } break;

            case PRINTF_STATE_CHECK_LENGTH:
            {
                if (*FormatString == 'h')
                {
                    LengthType = PRINTF_LENGTH_TYPE_SHORT;
                    State = PRINTF_STATE_SHORT_LENGTH;
                    FormatString++;
                }
                else if (*FormatString == 'l')
                {
                    LengthType = PRINTF_LENGTH_TYPE_LONG;
                    State = PRINTF_STATE_LONG_LENGTH;
                    FormatString++;
                }
                else
                {
                    State = PRINTF_STATE_CHECK_SPECIFIER;
                }
            } break;

            case PRINTF_STATE_SHORT_LENGTH:
            {
                if (*FormatString == 'h')
                {
                    LengthType = PRINTF_LENGTH_TYPE_SHORT_SHORT;
                    State = PRINTF_STATE_CHECK_SPECIFIER;
                    FormatString++;
                }
                else
                {
                    State = PRINTF_STATE_CHECK_SPECIFIER;
                }
            } break;

            case PRINTF_STATE_LONG_LENGTH:
            {
                if (*FormatString == 'l')
                {
                    LengthType = PRINTF_LENGTH_TYPE_LONG_LONG;
                    State = PRINTF_STATE_CHECK_SPECIFIER;
                    FormatString++;
                }
                else
                {
                    State = PRINTF_STATE_CHECK_SPECIFIER;
                }
            } break;

            case PRINTF_STATE_CHECK_SPECIFIER:
            {
                if (*FormatString == 'c')
                {
                    PutCharacter((char)*ArgPointer);
                    ArgPointer++;
                }
                else if (*FormatString == 's')
                {
                    if ((LengthType == PRINTF_LENGTH_TYPE_LONG) || (LengthType == PRINTF_LENGTH_TYPE_LONG_LONG))
                    {
                        PutFarString(*(const char far **)ArgPointer);
                        ArgPointer += 2;
                    }
                    else
                    {
                        PutString(*(const char **)ArgPointer);
                        ArgPointer++;
                    }
                }
                else if (*FormatString == '%')
                {
                    PutCharacter('%');
                }
                else if ((*FormatString == 'd') || (*FormatString == 'i'))
                {
                    Radix = 10;
                    Signed = TRUE;
                    ArgPointer = PrintNumber(ArgPointer, LengthType, Signed, Radix);
                }
                else if (*FormatString == 'u')
                {
                    Radix = 10;
                    Signed = FALSE;
                    ArgPointer = PrintNumber(ArgPointer, LengthType, Signed, Radix);
                }
                else if ((*FormatString == 'x') || (*FormatString == 'X') || (*FormatString == 'P'))
                {
                    Radix = 16;
                    Signed = FALSE;
                    ArgPointer = PrintNumber(ArgPointer, LengthType, Signed, Radix);
                }
                else if (*FormatString == 'o')
                {
                    Radix = 8;
                    Signed = FALSE;
                    ArgPointer = PrintNumber(ArgPointer, LengthType, Signed, Radix);
                }

                State = PRINTF_STATE_NORMAL;
                LengthType = PRINTF_LENGTH_TYPE_DEFAULT;
                Radix = 10;
                Signed = FALSE;

                FormatString++;
            } break;
        }
    }
}