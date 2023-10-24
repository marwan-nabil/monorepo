void PrintCharacter(char Character)
{
    X86PrintCharacter(Character, 0);
}

void PrintString(const char *String)
{
    while(*String)
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

int *printf_number(int* argp, int length, b8 sign, int radix)
{
    const char HexCharacters[] = "0123456789abcdef";
    char buffer[32];
    unsigned long long number;
    int number_sign = 1;
    int pos = 0;

    // process length
    switch (length)
    {
        case PRINTF_LENGTH_TYPE_SHORT_SHORT:
        case PRINTF_LENGTH_TYPE_SHORT:
        case PRINTF_LENGTH_TYPE_DEFAULT:
            if (sign)
            {
                int n = *argp;
                if (n < 0)
                {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long)n;
            }
            else
            {
                number = *(unsigned int*)argp;
            }
            argp++;
            break;

        case PRINTF_LENGTH_TYPE_LONG:
            if (sign)
            {
                long int n = *(long int*)argp;
                if (n < 0)
                {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long)n;
            }
            else
            {
                number = *(unsigned long int*)argp;
            }
            argp += 2;
            break;

        case PRINTF_LENGTH_TYPE_LONG_LONG:
            if (sign)
            {
                long long int n = *(long long int*)argp;
                if (n < 0)
                {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long)n;
            }
            else
            {
                number = *(unsigned long long int*)argp;
            }
            argp += 4;
            break;
    }

    // convert number to ASCII
    do 
    {
        u32 rem;
        X86Divide64BitsBy32Bits(number, radix, &number, &rem);
        buffer[pos++] = HexCharacters[rem];
    } while (number > 0);

    // add sign
    if (sign && number_sign < 0)
        buffer[pos++] = '-';

    // print number in reverse order
    while (--pos >= 0)
        PrintCharacter(buffer[pos]);

    return argp;
}

void _cdecl PrintFormatted(const char* fmt, ...)
{
    int* argp = (int*)&fmt;
    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_TYPE_DEFAULT;
    int radix = 10;
    b8 sign = FALSE;

    argp++;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_CHECK_LENGTH;
                                break;
                    default:    PrintCharacter(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_CHECK_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_TYPE_SHORT;
                                state = PRINTF_STATE_SHORT_LENGTH;
                                break;
                    case 'l':   length = PRINTF_LENGTH_TYPE_LONG;
                                state = PRINTF_STATE_LONG_LENGTH;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_SHORT_LENGTH:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_TYPE_SHORT_SHORT;
                    state = PRINTF_STATE_CHECK_SPECIFIER;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LONG_LENGTH:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_TYPE_LONG_LONG;
                    state = PRINTF_STATE_CHECK_SPECIFIER;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_CHECK_SPECIFIER:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   PrintCharacter((char)*argp);
                                argp++;
                                break;

                    case 's':   if (length == PRINTF_LENGTH_TYPE_LONG || length == PRINTF_LENGTH_TYPE_LONG_LONG) 
                                {
                                    PrintFarString(*(const char far**)argp);
                                    argp += 2;
                                }
                                else 
                                {
                                    PrintString(*(const char**)argp);
                                    argp++;
                                }
                                break;

                    case '%':   PrintCharacter('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = TRUE;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    case 'u':   radix = 10; sign = FALSE;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = FALSE;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    case 'o':   radix = 8; sign = FALSE;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_TYPE_DEFAULT;
                radix = 10;
                sign = FALSE;
                break;
        }

        fmt++;
    }
}