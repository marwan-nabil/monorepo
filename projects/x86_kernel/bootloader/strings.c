void PrintCharacter(char Character)
{
    x86_PrintCharacter(Character, 0);
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
    while(*String)
    {
        PrintCharacter(*String);
        String++;
    }
}

i32 *PrintNumber(i32 *ArgPointer, printf_length_type LengthType, b8 Signed, i32 Radix)
{
    const char HexCharacters[] = "0123456789abcdef";

    char LocalStringBuffer[32];
    u64 Number;
    i32 Sign = 1;
    i32 BufferPosition = 0;

    switch (LengthType)
    {
        case PRINTF_LENGTH_TYPE_SHORT_SHORT:
        case PRINTF_LENGTH_TYPE_SHORT:
        case PRINTF_LENGTH_TYPE_DEFAULT:
        {
            if (Signed)
            {
                i32 Value = *ArgPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    Sign = -1;
                }
                Number = (u64)Value;
            }
            else
            {
                Number = *(u32 *)ArgPointer;
            }
            ArgPointer++;
        } break;

        case PRINTF_LENGTH_TYPE_LONG:
        {
            if (Signed)
            {
                i32 Value = *(i32 *)ArgPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    Sign = -1;
                }
                Number = (u64)Value;
            }
            else
            {
                Number = *(u32 *)ArgPointer;
            }
            ArgPointer += 2;
        } break;

        case PRINTF_LENGTH_TYPE_LONG_LONG:
        {
            if (Signed)
            {
                i32 Value = *(i32 *)ArgPointer;
                if (Value < 0)
                {
                    Value = -Value;
                    Sign = -1;
                }
                Number = (u64)Value;
            }
            else
            {
                Number = *(u64 *)ArgPointer;
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
        PrintCharacter(LocalStringBuffer[BufferPosition]);
    }

    return ArgPointer;
}

void _cdecl PrintFormatted(const char *FormatString, ...)
{
    i32 *ArgPointer = (i32 *)&FormatString;
    printf_state State = PRINTF_STATE_NORMAL;
    printf_length_type LengthType = PRINTF_LENGTH_TYPE_DEFAULT;
    i32 Radix = 10;
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
                    PrintCharacter(*FormatString);
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
                    PrintCharacter((char)*ArgPointer);
                    ArgPointer++;
                }
                else if (*FormatString == 's')
                {
                    if ((LengthType == PRINTF_LENGTH_TYPE_LONG) || (LengthType == PRINTF_LENGTH_TYPE_LONG_LONG))
                    {
                        PrintFarString(*(const char far **)ArgPointer);
                        ArgPointer += 2;
                    }
                    else
                    {
                        PrintString(*(const char **)ArgPointer);
                        ArgPointer++;
                    }
                }
                else if (*FormatString == '%')
                {
                    PrintCharacter('%');
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