#pragma once

void _cdecl x86_PrintCharacter(char Character, u8 Page);
void _cdecl x86_Divide64By32
(
    unsigned long long Number, int Radix,
    unsigned long long *Quotient, u32 *Remainder
);