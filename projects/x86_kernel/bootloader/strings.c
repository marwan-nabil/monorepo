#include "projects\x86_kernel\platform\base_types.h"
#include "x86.h"

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